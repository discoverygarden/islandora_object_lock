<?php

function islandora_object_lock_admin_form() {
  $form = array();
  $form['islandora_object_lock_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable datastream locking'),
    '#default_value' => variable_get('islandora_object_lock_enabled', FALSE),
  );
  $form['islandora_object_lock_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Lock duration (minutes)'),
    '#default_value' => variable_get('islandora_object_lock_duration', 30),
  );

  return system_settings_form($form);
}