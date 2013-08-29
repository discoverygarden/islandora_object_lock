<?php

/**
 * @file
 * Utility functions for object locking module.
 */

/**
 * Alert user to the current object being locked.
 *
 * @global user
 *
 * @param AbstractObject $object
 *   The object that is locked.
 */
function islandora_object_lock_handle_possible_lock_notice(AbstractObject $object) {
  global $user;

  if (islandora_object_lock_is_locked($object) &&
      $user->name != islandora_object_lock_get_lock_username($object) &&
      islandora_object_lock_request_unlock_access()) {

    drupal_set_message(
      t("The object can't be edited as it is being modified by someone else. You may !print_link.",
        array('!print_link' => l(t('request that they unlock the object'), "islandora/object/{$object->id}/request_unlock"))
      ),
      'warning'
    );
  }
}
