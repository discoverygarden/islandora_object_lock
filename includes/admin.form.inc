<?php

/**
 * @file
 * Admin form for Islandora object locking.
 */

/**
 * Admin form.
 *
 * @param array $form
 *   An array representing a form within Drupal.
 * @param array $form_state
 *   An array containing the Drupal form state.
 *
 * @return array
 *   The form to be rendered.
 */
function islandora_object_lock_admin_form($form, $form_state) {
  module_load_include('inc', 'islandora_object_lock', 'includes/utilities');

  // Tabs.
  $form['islandora_object_lock_tabs'] = array(
    '#type' => 'vertical_tabs',
  );
  $form['islandora_object_lock_tabs']['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
  );
  $form['islandora_object_lock_tabs']['locks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Delete Locks'),
  );

  // Settings tab.
  $form['islandora_object_lock_tabs']['settings']['default'] = array(
    '#type' => 'radios',
    '#title' => t('Default global lock duration'),
    '#options' => array(
      'indefinite' => t('Indefinite'),
      'minutes' => t('Minutes'),
    ),
    '#default_value' => variable_get('islandora_object_lock_duration', 30) ? 'minutes' : 'indefinite',
  );
  $form['islandora_object_lock_tabs']['settings']['islandora_object_lock_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Global Lock duration (minutes)'),
    '#default_value' => variable_get('islandora_object_lock_duration', 30),
    '#description' => t('Unless overridden in individual collection management pages, this will apply to all objects and collections.'),
    '#states' => array(
      'visible' => array(
        ':input[name="default"]' => array('value' => 'minutes'),
      ),
    ),
  );
  $form['islandora_object_lock_tabs']['settings']['islandora_object_lock_cron_throttle'] = array(
    '#type' => 'textfield',
    '#title' => t('Lock release cron throttle'),
    '#default_value' => variable_get('islandora_object_lock_cron_throttle', 30),
    '#description' => t('The number of locks to release during each cron process. Adjust if cron tasks are not being completed.'),
  );
  $form['islandora_object_lock_tabs']['settings']['modified_hook'] = array(
    '#type' => 'checkbox',
    '#title' => t('Lift locks on datastream modified'),
    '#default_value' => variable_get('islandora_object_lock_lift_lock_on_modify', TRUE),
    '#description' => t('When enabled this will cause locks to automatically release themselves when a datastream is modified on an object.'),
  );

  // Locks tab.
  $locks_per_page = 10;
  $locks_count = islandora_object_lock_count_locks();
  $current_page = pager_default_initialize($locks_count, $locks_per_page);
  $form['islandora_object_lock_tabs']['locks']['islandora_object_locks_table'] = array(
    '#type' => 'tableselect',
    '#header' => array(
      'object' => t('Object'),
      'user' => t('User'),
      'expires' => t('Expires'),
    ),
    '#options' => islandora_object_lock_get_locks($locks_per_page * $current_page, $locks_per_page),
    '#multiple' => TRUE,
    '#empty' => t('No object locks.'),
  );

  $form['islandora_object_lock_tabs']['locks']['pager'] = array(
    '#type' => 'markup',
    '#markup' => theme('pager', array('quantity', count($locks_count))),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Validation for the admin form.
 *
 * @param array $form
 *   An array representing a form within Drupal.
 * @param array $form_state
 *   An array containing the Drupal form state.
 */
function islandora_object_lock_admin_form_validate($form, $form_state) {
  if ($form_state['values']['default'] == 'minutes') {
    if (empty($form_state['values']['islandora_object_lock_duration'])) {
      form_error($form['islandora_object_lock_tabs']['settings']['islandora_object_lock_duration'], t('Please specify a non-empty duration.'));
    }
    else {
      element_validate_integer_positive($form['islandora_object_lock_tabs']['settings']['islandora_object_lock_duration'], $form_state);
    }
  }
}

/**
 * Submit handler for the admin form.
 *
 * @param array $form
 *   An array representing a form within Drupal.
 * @param array $form_state
 *   An array containing the Drupal form state.
 */
function islandora_object_lock_admin_form_submit($form, $form_state) {
  if ($form_state['values']['default'] == 'indefinite') {
    $duration = FALSE;
  }
  else {
    $duration = $form_state['values']['islandora_object_lock_duration'];
  }
  variable_set('islandora_object_lock_duration', $duration);
  variable_set('islandora_object_lock_cron_throttle', $form_state['values']['islandora_object_lock_cron_throttle']);
  variable_set('islandora_object_lock_lift_lock_on_modify', $form_state['values']['modified_hook'] ? TRUE : FALSE);
  // Handle lifting of locks.
  $selected = array_filter($form_state['values']['islandora_object_locks_table']);
  if (!empty($selected)) {
    module_load_include('inc', 'islandora_object_lock', 'includes/utilities');
    foreach ($selected as $pid => $option) {
      islandora_object_lock_remove_object_lock($pid);
    }
  }
}
