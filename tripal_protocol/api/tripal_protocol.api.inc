<?php
/**
 *
 * @file
 * Provides an application programming interface (API) to manage protocols.
 */

/**
 * Returns a list of protocols to use in select lists.
 *
 * @param $syncd_only
 *   Whether or not to return all chado protocols or just those sync'd with
 *   drupal. Defaults to TRUE (only sync'd protocols).
 * @param $protocol_type
 *   The name of the protocol type that will be returned.
 *
 * @return
 *   An array of protocols sync'd with Drupal where each value is the protocol
 *   name and the keys are protocol_ids.
 *
 * @ingroup tripal_protocol_api
 */
function tripal_get_protocol_select_options(
  $syncd_only = TRUE,
  $protocol_type
) {
  $prot_list = [];
  $prot_list[] = 'Select a protocol';

  // Use this statement for getting the protocols.
  $csql = "SELECT P.name, P.protocol_id
      FROM {protocol} P ORDER BY name";
  //INNER JOIN {cvterm} CVT ON P.type_id = CVT.cvterm_id
  //    WHERE CVT.name = :protocol_type

  $protocols = chado_query($csql, [':protocol_type' => $protocol_type]);

  // Iterate through the protocols and build an array of all protocols including
  // those that are not synced.
  foreach ($protocols as $protocol) {
    $prot_list[$protocol->protocol_id] = $protocol->name;
  }
  return $prot_list;
}
