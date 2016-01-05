<?php
/**
 * 
 * @file
 * Provides an application programming interface (API) to manage arraydesigns.
 */

/**
 * Returns a list of arraydesigns to use in select lists.
 * 
 * @param $syncd_only
 *   Whether or not to return all chado arraydesigns or just those sync'd with drupal. Defaults
 *   to TRUE (only sync'd arraydesigns).
 * @return
 *   An array of arraydesigns sync'd with Drupal where each value is the arraydesign name
 *   and the keys are arraydesign_ids.
 * 
 * @ingroup tripal_arraydesign_api
 */
function tripal_get_arraydesign_select_options($syncd_only = TRUE) {
  $ad_list = array();
  $ad_list[''] = 'Select an array design';
  
  if ($syncd_only) {
    $sql = "SELECT *
      FROM public.chado_arraydesign CAD
        INNER JOIN {arraydesign} AD ON AD.arraydesign_id = CAD.arraydesign_id
      ORDER BY AD.name";
    $arraydesigns = chado_query($sql);
    
    // Iterate through the arraydesigns and build an array of those that are synced.
    foreach ($arraydesigns as $arraydesign) {
      $ad_list[$arraydesign->arraydesign_id] = $arraydesign->name;
    }
  }
  else {
    // Use this statement for getting the arraydesigns.
    $csql = "SELECT * FROM {arraydesign} ORDER BY name";
    $arraydesigns = chado_query($csql);
   
    // Iterate through the arraydesigns and build an array of all arraydesigns
    // including arraydesigns that are not synced.
    foreach ($arraydesigns as $arraydesign) {
      $ad_list[$arraydesign->arraydesign_id] = $arraydesign->name;
    }
  }
  return $ad_list;
}
