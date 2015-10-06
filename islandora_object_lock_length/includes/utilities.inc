<?php

/**
 * @file
 * Utilitie functions for manageing collection
 * level locks in the islandora_object_lock_length_locks
 * table created during install.
 */

/**
 * Create a collection lock.
 *
 * @param string $pid
 *   The pid of the collection to add the lock.
 *
 * @param string $duration
 *   The length of the lock duration.
 */
function islandora_object_lock_length_add_collection_lock($pid, $duration) {
  db_insert('islandora_object_lock_length_locks')
    ->fields(array(
      'pid' => $pid,
      'duration' => $duration,
    ))
    ->execute();
}

/**
 * Remove a collection lock.
 *
 * @param string $pid
 *   The identifier of the collection lock to delete.
 */
function islandora_object_lock_length_remove_collection_lock($pid) {
  db_delete('islandora_object_lock_length_locks')
  ->condition('pid', $pid, '=')
  ->execute();
}

/**
 * Get a collection lock.
 *
 * @param string $pid
 *   The pid of the collection lock to get.
 */
function islandora_object_lock_length_get_collection_lock($pid) {
  $lock = db_select('islandora_object_lock_length_locks', 'n')
  ->fields('n', array('duration'))
  ->condition('pid', $pid, '=')
  ->execute();
  $data = $lock->fetchObject();
  return $data;
}
