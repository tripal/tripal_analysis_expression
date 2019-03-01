<?php

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
function tripal_analysis_expression_get_protocol_select_options(
  $protocol_type
) {
  $prot_list = [];
  $prot_list[] = 'Select a protocol';

  // Use this statement for getting the protocols.
  $csql = "SELECT P.name, P.protocol_id
      FROM {protocol} P ORDER BY name";

  $protocols = chado_query($csql, [':protocol_type' => $protocol_type]);

  // Iterate through the protocols and build an array of all protocols including
  // those that are not synced.
  foreach ($protocols as $protocol) {
    $prot_list[$protocol->protocol_id] = $protocol->name;
  }
  return $prot_list;
}

/**
 * Copies the uploaded file to public://expression.
 * This will create the directory if it does not exist.
 *
 * @param $uri
 *  URI to the file.
 * @return string
 *  Returns the file path to the new location of the file.
 */
function tripal_analysis_expression_cache_file($file_path) {
  $destination = 'public://expression';
  if (!file_prepare_directory($destination, FILE_CREATE_DIRECTORY)) {
    print ("Could not create directory at $destination, please check your permissions and try again.\n");
    // return false;
  }

  $file_name = pathinfo($file_path)['basename'];
  $new_path = $destination . '/' . uniqid() . '_' . $file_name;

  if (!copy($file_path, $new_path)) {
    print ("Could not copy $file_path to $new_path.");
    return false;
  }

  return $new_path;
}

/**
 * Add the file path as a property of the analysis.
 *
 * @param $analysis_id
 * @param $file_path
 * @return bool|\DatabaseStatementInterface|int
 * @throws \Exception
 */
function tripal_analysis_expression_add_file_as_prop($analysis_id, $file_path) {
  $cvterm = chado_insert_cvterm([
    'id' => 'local:data_source_file',
    'name' => 'data_source_file',
    'cv_name' => 'local',
  ]);

  // analysis_id in analysisprop must be unique or job will fail
  $analysis = db_select('chado.analysisprop', 'a')
    ->fields('a')
    ->condition('analysis_id', $analysis_id, '=')
    ->execute()
    ->fetchAll();

  if ($analysis) {
    print("$file_path could not be added to the analysisprop table because an analysis with id $analysis_id already exists.\n");
    return false;
  }

  $id = db_insert('chado.analysisprop')
    ->fields([
      'analysis_id' => $analysis_id,
      'type_id' => $cvterm->cvterm_id,
      'value' => $file_path,
    ])
    ->execute();

  return $id ? $id : false;
}


// PVALUE TEST FUNCTIONS, NOT FOR USE
function tripal_analysis_expression_get_pvalue_id() {
  return 344;
}

function tripal_analysis_expression_get_evidence_code_id() {
  return 343;
}

