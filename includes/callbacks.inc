<?php

/**
 * @file
 * Callbacks for object locking module.
 */

/**
 * Tries to send out an email asking for an object to be unlocked.
 *
 * @global user
 *
 * @param AbstractObject $object
 *   The object that is locked
 */
function islandora_object_lock_request_unlock(AbstractObject $object) {
  module_load_include('inc', 'islandora_object_lock', 'includes/utilities');
  global $user;

  if (islandora_object_lock_is_locked($object)) {
    // Email owner of lock.
    $account = user_load_by_name(islandora_object_lock_get_lock_username($object));
    drupal_mail(
      'islandora_object_lock',
      'request_unlock',
      $account->mail,
      user_preferred_language($account),
      array('object' => $object)
    );
    // User feedback.
    drupal_set_message(t("The request to unlock the object has been sent."));
  }
  else {
    drupal_set_message(t("The object was not locked, no request sent."), 'warning');
  }

  drupal_goto(t("islandora/object/@id", array('@id' => $object->id)));
}
