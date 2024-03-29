<?php

/**
 * @file
 */

/**
 * Returns a list of arraydesigns to use in select lists.
 *
 * @return
 *   An array of arraydesigns sync'd with Drupal where each value is the
 *   arraydesign name and the keys are arraydesign_ids.
 *
 * @ingroup tripal_arraydesign_api
 */
function tripal_analysis_expression_get_arraydesign_select_options() {
  $ad_list = [];
  $ad_list[''] = 'Select an array design';

  // Use this statement for getting the arraydesigns.
  $csql = "SELECT * FROM {arraydesign} ORDER BY name";
  $arraydesigns = chado_query($csql);

  // Iterate through the arraydesigns and build an array of all arraydesigns
  // including arraydesigns that are not synced.
  foreach ($arraydesigns as $arraydesign) {
    $ad_list[$arraydesign->arraydesign_id] = $arraydesign->name;
  }
  return $ad_list;
}

/**
 * Returns a list of protocols to use in select lists.
 *
 * @param $protocol_type
 *   The name of the protocol type that will be returned.
 *
 * @return
 *   An array of protocols sync'd with Drupal where each value is the protocol
 *   name and the keys are protocol_ids.
 *
 * @ingroup tripal_protocol_api
 */
function tripal_analysis_expression_get_protocol_select_options($protocol_type) {
  $prot_list = [];
  $prot_list[] = 'Select a protocol';

  // Use this statement for getting the protocols.
  $csql = "SELECT P.name, P.protocol_id
      FROM {protocol} P ORDER BY name";

  $protocols = chado_query($csql, []);

  // Iterate through the protocols and build an array of all protocols including
  // those that are not synced.
  foreach ($protocols as $protocol) {
    $prot_list[$protocol->protocol_id] = $protocol->name;
  }
  return $prot_list;
}
