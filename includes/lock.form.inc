<?php

//@TODO: make confirm_form()
function islandora_object_lock_form_lock($form, $form_state, $object, $op) {
  $form = array();
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Lock'),
  );
  $form['pid'] = array(
    '#type' => 'value',
    '#value' => $object->id,
  );
  $form['lock-op'] = array(
    '#type' => 'value',
    '#value' => $op,
  );
  return $form;
}

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
