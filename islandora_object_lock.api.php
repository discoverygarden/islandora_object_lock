<?php
/**
 * @file
 * Hook definitions.
 */

/**
 * Mask datastreams from the "Lift locks on datastream modified" feature.
 *
 * @param AbstractDatastream $datastream
 *   The object on which a datastream was modified.
 *
 * @return string[]
 *   An array of datastream identifiers to ignore.
 */
function hook_islandora_object_lock_ignored_datastream_modifications(AbstractDatastream $datastream) {
  return array(
    'RELS-EXT',
    'MY-FUNKY-DS',
  );
}

/**
 * Mask datastreams from the "Lift locks on datastream modified" feature.
 *
 * Model-restricted variant of
 * hook_islandora_object_lock_ignored_datastream_modifications().
 *
 * @see hook_islandora_object_lock_ignored_datastream_modifications()
 */
function hook_CMODEL_PID_islandora_object_lock_ignored_datastream_modifications(AbstractDatastream $datastream) {
}
