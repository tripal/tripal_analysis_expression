<?php
/**
 *
 * @file
 * Functions to interact with biomaterials.
 *
 * @ingroup tripal_biomaterial
 */

/**
 * This function is intented to be used in autcomplete forms for biomaterials.
 *
 * @param $text
 *   The string to search for.
 *
 * @return
 *   A json array of terms that begin with the provided string.
 *
 * @ingroup tripal_biomaterial_api
 */
function tripal_autocomplete_biomaterial($text) {
  $matches = [];

  $sql = "SELECT * FROM {biomaterial} WHERE lower(name) like lower(:name) ";
  $args = [];
  $args[':name'] = $text . '%';
  $sql .= "ORDER BY name ";
  $sql .= "LIMIT 25 OFFSET 0 ";
  $results = chado_query($sql, $args);
  $items = [];
  foreach ($results as $biomaterial) {
    // Don't include null biomaterials
    if ($biomaterial->name == 'null') {
      continue;
    }
    $items[$biomaterial->name] = $biomaterial->name;
  }
  drupal_json_output($items);
}
