<?php
/**
 * @file
 * Form handlers dealing with our locks.
 */

/**
 * Form validation handler.
 *
 * Check that we have the lock; otherwise, we will be redirected with an error
 * message output.
 */
function islandora_object_lock_check_lock_validation_handler(&$form, &$form_state) {
  $info = $form_state['islandora_object_lock_info'];
  $object = islandora_object_load($info['pid']);
  islandora_object_lock_handle_possible_unlock_form_error($object, $info['suffix'], $info['allow_acquisition']);
}

/**
 * Form submission handler.
 *
 * Release the lock (assuming we have it).
 */
function islandora_object_lock_release_lock_submit_handler(&$form, &$form_state) {
  islandora_object_lock_remove_object_lock($form_state['islandora_object_lock_info']['pid']);
}

/**
 * Helper to inject our lock validation and release logic.
 *
 * @param array $form
 *   The form structure being output.
 * @param array $form_state
 *   The form state for the given form.
 * @param AbstractObject $object
 *   The object being modified by the form process.
 * @param string|NULL $dsid
 *   A string indicating which datastream is being modified, or NULL to
 *   indicate that it is the object being modified.
 * @param string $path_suffix
 *   A path suffix relative to 'islandora/object/%islandora_object'
 *   to use when redirecting.
 * @param bool $allow_acquisition
 *   Try to acquire the lock if the current user does not have it; the lock
 *   will have to be manually acquired.
 */
function islandora_object_lock_acquire_during_alter(&$form, &$form_state, AbstractObject $object, $dsid = NULL, $path_suffix = NULL, $allow_acquisition = FALSE) {
  form_load_include($form_state, 'inc', 'islandora_object_lock', 'includes/utilities');
  form_load_include($form_state, 'inc', 'islandora_object_lock', 'includes/form_handlers');

  $locked = islandora_object_lock_is_locked($object);
  if ($locked) {
    // Object is locked...
    if (islandora_object_lock_user_has_lock($object)) {
      // ... and you have it! Jolly good!
      drupal_set_message(t('You have the lock on this object, which you can <a href="@release_url">release</a>.', array(
        '@release_url' => url("islandora/object/{$object->id}/manage/datastreams/locking/unlock", array(
          'query' => array(
            'destination' => $allow_acquisition ? "islandora/object/{$object->id}/manage/datastreams" : "islandora/object/{$object->id}/$path_suffix",
          ),
        )),
      )), 'status', FALSE);
    }
    else {
      // ... but somebody else has it ...
      $manage_page = "islandora/object/$object->id/manage";
      $message = t('This object has been locked by another user.');
      if (islandora_object_lock_request_unlock_access()) {
        // ... but you can ask them to release it.
        $message .= ' ' . t('You can request that they <a href="@release_url">release</a> the lock.', array(
          '@release_url' => url("islandora/object/{$pid}/request_unlock", array(
            'query' => array(
              'destination' => $manage_page,
            ),
          )),
        ));
      }
      drupal_set_message(filter_xss($message), 'error');
      drupal_goto($manage_page);
    }
  }
  elseif ($allow_acquisition) {
    // Object is not locked, and have been instructed to automatically
    // acquire/release it
    islandora_object_lock_set_object_lock($object->id);
    $form['#submit'][] = 'islandora_object_lock_release_lock_submit_handler';
  }
  else {
    if (islandora_object_lock_access_lock($object, 'lock')) {
      drupal_set_message(t('You must <a href="@lock_url">acquire the lock</a> before any modifications can be made with this form.', array(
        '@lock_url' => url("islandora/object/{$object->id}/manage/datastreams/locking/lock", array(
          'query' => array(
            'destination' => isset($path_suffix) ? "islandora/object/{$object->id}/$path_suffix" : NULL,
          ),
        )),
      )), 'warning');
    }
    else {
      drupal_set_message(t('This form requires the object to be locked; however, you do not have permission to lock the object.'), 'error');
    }
  }

  // Dump some info into the $form_state we will later use in submission and
  // validation handlers.
  $form_state['islandora_object_lock_info'] = array(
    'pid' => $object->id,
    'dsid' => $dsid,
    'suffix' => $path_suffix,
    'allow_acquisition' => $allow_acquisition,
  );

  $form['#validate'][] = 'islandora_object_lock_check_lock_validation_handler';
}
