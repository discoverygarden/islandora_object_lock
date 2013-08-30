<?php

/**
 * @file
 * Object locking form.
 */

/**
 * Lock/unlock form.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 * @param AbstractFedoraObject $object
 *   The Fedora object.
 * @param string $op
 *   The form operation, may be: lock, unlock.
 *
 * @return mixed
 *   The form.
 */
function islandora_object_lock_form_lock($form, $form_state, AbstractFedoraObject $object, $op) {
  $form = array();
  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $object->id,
  );
  $form['lock-op'] = array(
    '#type' => 'value',
    '#value' => $op,
  );

  if ($op == 'lock') {
    $question = t('Lock this object?');
    $description = t('This temporarily prevents object modifications by other users.');
  }
  else {
    $question = t('Remove the object lock?');
    $description = t('This allows other users to modify the object.');
  }
  return confirm_form($form, check_plain($question), "islandora/object/$object->id/manage/datastreams", check_plain($description));
}

/**
 * Submit handler for the lock/unlock form.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function islandora_object_lock_form_lock_submit($form, &$form_state) {
  $pid = $form_state['values']['pid'];
  if ($form_state['values']['lock-op'] == 'lock') {
    islandora_object_lock_set_object_lock($pid);
    drupal_set_message(t('The object has been locked.'));
  }
  elseif ($form_state['values']['lock-op'] == 'unlock') {
    islandora_object_lock_remove_object_lock($pid);
    drupal_set_message(t('The object lock has been removed.'));
  }
  $form_state['redirect'] = "islandora/object/$pid/manage/datastreams";
}

/**
 * Theme the object lock management table.
 *
 * @param array $variables
 *   Variables passed to this theme function.
 *
 * @return string
 *   Markup representing the table for rendering.
 */
function theme_islandora_object_lock_policy_management_table(array $variables) {
  // Manually add the table select javascript as we are using a custom table.
  drupal_add_js('misc/tableselect.js');
  $table = $variables['table'];
  $row_elements = $table['rows'];
  $rows = array();
  foreach (element_children($row_elements) as $key) {
    $columns = array();
    $row_element = $row_elements[$key];
    foreach (element_children($row_element) as $key) {
      $column_element = $row_element[$key];
      $columns[] = array(
        'data' => drupal_render($column_element),
        'class' => isset($cell_element['#attributes']['class']) ? $column_element['#attributes']['class'] : NULL,
      );
    }
    $rows[] = $columns;
  }
  $variables = array(
    'header' => $table['#header'],
    'rows' => $rows,
    'attributes' => $table['#attributes'],
    'caption' => NULL,
    'colgroups' => NULL,
    'sticky' => NULL,
    'empty' => t("No Locks Found."),
  );
  return theme('table', $variables);
}
