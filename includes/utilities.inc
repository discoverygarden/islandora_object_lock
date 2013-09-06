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

/**
 * Will get a number of object locks.
 *
 * @param int $offset
 *   The offset for the object lock query.
 * @param int $limit
 *   The limit for the object lock query.
 *
 * @return array
 *   The locks formated for options in a tableselect.
 */
function islandora_object_lock_get_locks($offset, $limit) {
  $tuque = islandora_get_tuque_connection();

  $repository = $tuque->repository;
  $query = "
    PREFIX islandora_rels_ext: <" . ISLANDORA_RELS_EXT_URI . ">
    PREFIX fm: <" . FEDORA_MODEL_URI . ">
    SELECT ?label ?object ?expiry ?user FROM <#ri> WHERE {
      ?object islandora_rels_ext:" . ISLANDORA_OBJECT_LOCK_USER_PRED . " ?user;
        islandora_rels_ext:" . ISLANDORA_OBJECT_LOCK_EXPIRY_PRED . " ?expiry
    OPTIONAL{?object fm:label ?label}
    }
    ORDER BY ?expiry
    LIMIT $limit
    OFFSET $offset
  ";

  $raw_results = $repository->ri->sparqlQuery($query);
  $results = array();
  foreach ($raw_results as $raw_result) {
    $results[$raw_result['object']['value']] = array(
      'object' => l($raw_result['label']['value'], "islandora/object/{$raw_result['object']['value']}"),
      'expires' => format_date(strtotime($raw_result['expiry']['value'])),
      'user' => $raw_result['user']['value'],
    );
  }

  return $results;
}

/**
 * Will get how many object locks there are.
 *
 * @return int
 *   The number of object locks existing.
 */
function islandora_object_lock_count_locks() {
  $tuque = islandora_get_tuque_connection();
  $repository = $tuque->repository;

  $query = "
    PREFIX islandora_rels_ext: <" . ISLANDORA_RELS_EXT_URI . ">
    SELECT ?object FROM <#ri> WHERE {
      ?object islandora_rels_ext:" . ISLANDORA_OBJECT_LOCK_USER_PRED . " ?user
    }
  ";

  return $repository->ri->countQuery($query, 'sparql');
}

/**
 * Check if an object is locked.
 *
 * @param AbstractObject $object
 *   The object to check.
 *
 * @return bool
 *   TRUE if the object is locked.
 */
function islandora_object_lock_is_locked(AbstractObject $object) {
  $lock = $object->relationships->get(ISLANDORA_RELS_EXT_URI, ISLANDORA_OBJECT_LOCK_EXPIRY_PRED, NULL, RELS_TYPE_PLAIN_LITERAL);
  if (count($lock) < 1) {
    return FALSE;
  }
  else {
    $expiry_time = strtotime($lock[0]['object']['value']);
    if ($expiry_time < time()) {
      islandora_object_lock_remove_object_lock($object->id);
      return FALSE;
    }
    else {
      return TRUE;
    }
  }
}

/**
 * Triggers a form error if the object is not locked.
 *
 * @param AbstractObject $object
 *   The object to check.
 * @param string $form_element
 *   A form element to call the form_set_error on.
 */
function islandora_object_lock_handle_possible_unlock_form_error(AbstractObject $object, $form_element) {
  if (!islandora_object_lock_is_locked($object)) {
    form_set_error($form_element, t('This object needs to be locked for editing.'));
  }
}
