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
  }
  else {
    $question = t('Remove the object lock?');
  }
  return confirm_form($form, check_plain($question), "islandora/object/$object->id/manage/datastreams");
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
    drupal_set_message(t('Object locked'));
  }
  elseif ($form_state['values']['lock-op'] == 'unlock') {
    islandora_object_lock_remove_object_lock($pid);
    drupal_set_message(t('Object lock removed'));
  }
  $form_state['redirect'] = "islandora/object/$pid/manage/datastreams";
}
