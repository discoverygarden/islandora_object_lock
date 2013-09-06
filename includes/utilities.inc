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

/**
 * Create a collection lock.
 *
 * @param string $pid
 *   The pid of the collection to add the lock.
 *
 * @param string $duration
 *   The length of the lock duration.
 */
function islandora_object_lock_add_collection_lock($pid, $duration) {
  db_insert('islandora_object_lock_locks')
  ->fields(array('pid' => $pid))
  ->fields(array('duration' => $duration))
  ->execute();
}

/**
 * Edit a collection lock.
 *
 * @param string $pid
 *   The pid of the collection to edit the lock.
 *
 * @param string $duration
 *   The length of the lock duration.
 */
function islandora_object_lock_edit_collection_lock($pid, $duration) {
  db_update('islandora_object_lock_locks')
  ->fields(array('duration' => $duration))
  ->condition('pid', $pid, '=')
  ->execute();
}

/**
 * Remove a collection lock.
 *
 * @param string $pid
 *   The identifier of the collection lock to delete.
 */
function islandora_object_lock_remove_collection_lock($pid) {
  db_delete('islandora_object_lock_locks')
  ->condition('pid', $pid, '=')
  ->execute();
}
/**
 * Get a collection lock.
 *
 * @param string $pid
 *   The pid of the collection lock to get.
 */
function islandora_object_lock_get_collection_lock($pid) {
  $lock = db_select('islandora_object_lock_locks', 'n')
  ->fields('n', array('duration'))
  ->condition('pid', $pid, '=')
  ->execute();
  $data = $lock->fetchObject();
  return $data;
}
