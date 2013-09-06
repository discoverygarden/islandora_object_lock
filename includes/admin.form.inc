<?php

/**
 * @file
 * Admin form for Islandora object locking.
 */

/**
 * Admin form.
 *
 * @return mixed
 *   Return the form array.
 */
function islandora_object_lock_admin_form() {
  module_load_include('inc', 'islandora_object_lock', 'includes/utilities');
  $form = array();

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
  $form['islandora_object_lock_tabs']['settings']['islandora_object_lock_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Global Lock duration (minutes)'),
    '#default_value' => variable_get('islandora_object_lock_duration', 30),
    '#description' => t('Unless overridden in individual collection management pages, this will apply to all objects and collections.'),
  );
  $form['islandora_object_lock_tabs']['settings']['islandora_object_lock_cron_throttle'] = array(
    '#type' => 'textfield',
    '#title' => t('Lock release cron throttle'),
    '#default_value' => variable_get('islandora_object_lock_cron_throttle', 30),
    '#description' => t('The number of locks to release during each cron process. Adjust if cron tasks are not being completed.'),
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

  $form['#submit'][] = 'islandora_object_lock_manage_locks_submit';
  $form['islandora_object_lock_tabs']['locks']['pager'] = array(
    '#type' => 'markup',
    '#markup' => theme('pager', array('quantity', count($locks_count))),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form['#submit'][] = 'system_settings_form_submit';
  $form['#theme'] = 'system_settings_form';
  return $form;
}

/**
 * This will delete the indicated locks.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function islandora_object_lock_manage_locks_submit($form, $form_state) {
  module_load_include('inc', 'islandora_object_lock', 'includes/utilities');

  foreach ($form_state['values']['islandora_object_locks_table'] as $pid => $option) {
    if ($option) {
      islandora_object_lock_remove_object_lock($pid);
    }
  }
}
