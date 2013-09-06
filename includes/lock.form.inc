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
  module_load_include('inc', 'islandora_object_lock', 'includes/utilities');

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
