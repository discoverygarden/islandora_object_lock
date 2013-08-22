<?php

/**
 * @file
 * Admin form for Islandora object locking.
 */

/**
 * Admin form.
 *
 * @return mixed
 *   Return the form array.
 */
function islandora_object_lock_admin_form() {
  $form = array();
  $form['islandora_object_lock_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Lock duration (minutes)'),
    '#default_value' => variable_get('islandora_object_lock_duration', 30),
  );
  $form['islandora_object_lock_cron_throttle'] = array(
    '#type' => 'textfield',
    '#title' => t('Lock release cron throttle'),
    '#default_value' => variable_get('islandora_object_lock_cron_throttle', 30),
    '#description' => t('The number of locks to release during each cron process. Adjust if cron tasks are not being completed.'),
  );
  return system_settings_form($form);
}
