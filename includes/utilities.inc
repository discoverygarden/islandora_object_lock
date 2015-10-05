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
 * Helper to check if the current user has the lock.
 *
 * @param AbstractObject $object
 *   The object to be checked.
 *
 * @return bool
 *   TRUE if the current user has a lock on the object; otherwise, FALSE.
 */
function islandora_object_lock_user_has_lock(AbstractObject $object) {
  global $user;

  return islandora_object_lock_is_locked($object) && $user->name == islandora_object_lock_get_lock_username($object);
}

/**
 * Triggers a form error if the object is not locked.
 *
 * @param AbstractObject $object
 *   The object to check.
 * @param string|NULL $path_suffix
 *   A path relative to "islandora/object/%islandora_object" which can be used
 *   to "retry" an operation.
 */
function islandora_object_lock_handle_possible_unlock_form_error(AbstractObject $object, $path_suffix = NULL) {
  $locked = islandora_object_lock_is_locked($object);
  $owned = islandora_object_lock_user_has_lock($object);

  $manage_page = "islandora/object/$object->id/manage/datastreams";

  if (!$owned) {
    if ($locked) {
      $message = t('This object has been locked by another user.');
      if (islandora_object_lock_request_unlock_access()) {
        $message .= ' ' . t('You can request that they <a href="@release_url">release</a> the lock.', array(
          '@release_url' => url("islandora/object/{$object->id}/request_unlock", array(
            'query' => array(
              'destination' => $manage_page,
            ),
          )),
        ));
      }
    }
    elseif ($path_suffix !== NULL) {
      $message = t('Your lock has expired due to inactivity. To re-lock this object, please <a href="@edit_url">try again</a>. Any changes since your last save will be lost.', array(
        '@edit_url' => url("islandora/object/{$object->id}/{$path_suffix}"),
      ));
    }
    else {
      $message = t('Your lock has expired due to inactivity. To re-lock this object, please <a href="@edit_url">try again</a>.', array(
        '@edit_url' => url("islandora/object/{$object->id}/manage/datastreams/locking/lock"),
      ));
    }
    form_set_error(NULL, filter_xss($message));
    drupal_goto($manage_page);
  }
}
