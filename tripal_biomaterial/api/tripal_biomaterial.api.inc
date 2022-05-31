<?php
/**
 *
 * @file
 * Functions to interact with biomaterials.
 *
 * @ingroup tripal_biomaterial
 */

function create_unique_arraydesign($arraydesign_id, $organism_id, $analysis_id) {
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id',
    [':analysis_id' => $analysis_id])->fetchField();

  // Grab every field except the foreign key.
  $columns = [
    'name',
    'version',
    'description',
    'array_dimensions',
    'element_dimensions',
    'num_of_elements',
    'num_array_columns',
    'num_array_rows',
    'num_grid_columns',
    'num_grid_rows',
    'num_sub_columns',
    'num_sub_rows',
    'manufacturer_id',
    'platformtype_id',
    'platformtype_id',
    'dbxref_id',
    'dbxref_id',
  ];

  $select_values = ['arraydesign_id' => $arraydesign_id];
  $values = chado_select_record('arraydesign', $columns, $select_values);
  $values = (array) $values;

  // Update the name field to create a unique record.
  $values['name'] = $values['name'] . ' - Expression analysis: ' . $analysis . ' of organism: ' . $organism_g . ' ' . $organism_s;

  $arraydesign = chado_insert_record('arraydesign', $values);
  $arraydesign_id = $arraydesign['arraydesign_id'];
  return $arraydesign_id;
}

function create_unique_assay($assay_id, $contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id) {
  $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id',
    [':biomaterial_id' => $biomaterial_id])->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id',
    [':analysis_id' => $analysis_id])->fetchField();

  // Grab every field except the foreign key.
  $columns = [
    'assaydate',
    'arrayidentifier',
    'arraybatchidentifier',
    'name',
    'description',
    'arraydesign_id',
    'operator_id',
    'dbxref_id',
    'protocol_id',
  ];

  $select_values = ['assay_id' => $assay_id];
  $values = chado_select_record('assay', $columns, $select_values);
  $values = (array) $values;

  // Update the name field to create a unique record.
  $values['name'] = $values['name'] . ' - ' . $biomaterial . ' - ' . $organism_g . ' ' . $organism_s . ' - ' . $analysis;

  $assay = chado_insert_record('assay', $values);
  $assay_id = $assay['assay_id'];
  return $assay_id;
}

function create_unique_acquisition($acquisition_id, $assay_id, $biomaterial_id, $organism_id, $analysis_id) {
  $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id',
    [':biomaterial_id' => $biomaterial_id])->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id',
    [':analysis_id' => $analysis_id])->fetchField();

  // Grab every field except the foreign key.
  $columns = [
    'acquisitiondate',
    'name',
    'uri',
    'assay_id',
    'channel_id',
    'protocol_id',
  ];

  $select_values = ['acquisition_id' => $acquisition_id];
  $values = chado_select_record('acquisition', $columns, $select_values);
  $values = (array) $values;

  // Update the name field to create a unique record.
  $values['name'] = $values['name'] . ' - ' . $biomaterial . ' - ' . $organism_g . ' ' . $organism_s . ' - ' . $analysis;
  $values['assay_id'] = $assay_id;

  $acquisition = chado_insert_record('acquisition', $values);
  $acquisition_id = $acquisition['acquisition_id'];
  return $acquisition_id;
}

function create_unique_quantification($quantification_id, $acquisition_id, $biomaterial_id, $organism_id, $analysis_id) {
  $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id',
    [':biomaterial_id' => $biomaterial_id])->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id',
    [':organism_id' => $organism_id])->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id',
    [':analysis_id' => $analysis_id])->fetchField();

  // Grab every field except the foreign key.
  $columns = [
    'quantificationdate',
    'name',
    'uri',
    'acquisition_id',
    'analysis_id',
    'operator_id',
    'protocol_id',
  ];

  $select_values = ['quantification_id' => $quantification_id];
  $values = chado_select_record('quantification', $columns, $select_values);
  $values = (array) $values;

  // Update the name field to create a unique record.
  $values['name'] = $values['name'] . ' - ' . $biomaterial . ' - ' . $organism_g . ' ' . $organism_s . ' - ' . $analysis;
  $values['acquisition_id'] = $acquisition_id;

  $quantification = chado_insert_record('quantification', $values);
  $quantification_id = $quantification['quantification_id'];
  return $quantification_id;
}

/**
 *
 * Implements a function to create a biomaterial. The name field in the
 * biomaterial, is the only field that is required to create a biomaterial
 * record. However, becuase this is designed to be the primary method by
 * biomaterials are loaded into the table, the function provides extra
 * parameters with which to describe the biomaterial.
 *
 * @param $biomaterial
 *  The name of the biomaterial for which a record will be created.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis associated with the experiment.
 *
 * @param $organism_id
 *  The organism_id of the organism associated with the experiment.
 *
 * @param $biosourceprovider_id
 *  The contact_id of the contact who provided the biomaterial.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_biomaterial_create_biomaterial($biomaterial, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_id, $biomaterial_description, $warnings = TRUE) {
  $biomaterial_result = chado_query('select biomaterial_id, taxon_id, description from {biomaterial} where name = :name limit 1',
    [':name' => $biomaterial])->fetchObject();

  $biomaterial_id = NULL;

  if ($biomaterial_result) {

    // if sample_name already exists in chado db, print warning message
    if ($biomaterial_result->taxon_id && $warnings) {
      print("WARNING: Biomaterial with sample_name of '$biomaterial' alreay exists in the database. Overwriting database entries for $biomaterial...\n");
      if ($biomaterial_result->taxon_id and $biomaterial_result->taxon_id != $organism_id) {
        $analysis_taxon = chado_get_organism_scientific_name(chado_get_organism(['organism_id' => $organism_id], []));
        $biomaterial_taxon = chado_get_organism_scientific_name(chado_get_organism(['organism_id' => $biomaterial_result->taxon_id], []));
        print "ERROR: Mismatch between the organism selected in the loader " .
          "and the organism of a biomaterial in the Chado database.\n" .
          "  Organism selected by loader: " . $analysistaxon . "\n" .
          "  Biomaterial organism: " . $biomaterial_taxon . "\n" .
          "  On biomaterial: " . $biomaterial . "\n" .
          "Please ensure that the organism selected by the loader " .
          "and the organism for each biomaterial in the file to be loaded " .
          "are the same. If these organisms should not be the same, " .
          "delete the offending biomaterial in Chado or change the name of " .
          "the biomaterial to another unique name.\n";
        exit;
      }
    }

    $biomaterial_id = $biomaterial_result->biomaterial_id;

    //overwrite the existing description value
    if ($biomaterial_description == '' and $biomaterial_result->description != '') {
      $biomaterial_description = '';
    }
  }

  //if description is blank but we have an analysis, use that instead.
  if ($analysis_id && $biomaterial_description == '') {
    $analysis_name = chado_select_record('analysis', ['name'],
      ['analysis_id' => $analysis_id]);
    $biomaterial_description = 'This biomaterial: ' . $biomaterial . ', was created for the analysis: ' . $analysis_name[0]->name;
  }

  $values = [
    'name' => $biomaterial,
    'taxon_id' => $organism_id,
  ];

  if ($dbxref_id) {
    $values['dbxref_id'] = $dbxref_id;
  }

  if ($biomaterial_description) {
    $values['description'] = $biomaterial_description;
  }

  if ($biosourceprovider_id) {
    $values['biosourceprovider_id'] = $biosourceprovider_id;
  }

  if (!$biomaterial_id) {
    $biomaterial = chado_insert_record('biomaterial', $values);
    if ($biomaterial == FALSE) {
      print "ERROR: Unable to write to biomaterial table\n";
      exit;
    }
  }

  else {
    $biomaterial = chado_update_record('biomaterial', ['name' => $biomaterial],
      $values, ['return_record' => TRUE]);
    if ($biomaterial == FALSE) {
      print "ERROR: Unable to update biomaterial_table\n";
      exit;
    }
  }
  $biomaterial_id = $biomaterial['biomaterial_id'];

  return $biomaterial_id;
}

/**
 * @param $contact_name
 *Creates a new biomaterial contact
 *
 * @return mixed
 */
function tripal_biomaterial_create_biomaterial_contact($contact_name) {
  $contact_id = chado_query('select contact_id from {contact} where name = :name limit 1',
    [':name' => $contact_name])->fetchField();
  if (!$contact_id) {
    $values = [
      'name' => $contact_name,
    ];
    $contact = chado_insert_record('contact', $values);
    if ($contact == FALSE) {
      print "ERROR: Unable to write to contact table\n";
      exit;
    }
    $contact_id = $contact['contact_id'];
  }
  return $contact_id;
}

function tripal_biomaterial_create_ncbi_db($url_name, $db_name, $db_description) {

  $url = "http://www.ncbi.nlm.nih.gov/";
  $urlprefix = "http://www.ncbi.nlm.nih.gov/" . $url_name . "/";

  $values = [
    'name' => $db_name,
    'description' => $db_description,
    'urlprefix' => $urlprefix,
    'url' => $url,
  ];

  $db_id = chado_query('select db_id from {db} where name = :name limit 1',
    [':name' => $db_name])->fetchField();

  if (!$db_id) {
    $db = chado_insert_record('db', $values);
    if ($db == FALSE) {
      print "ERROR: Unable to write to db table\n";
      exit;
    }
    $db_id = $db['db_id'];
  }

  if ($db_id) {
    $db = chado_update_record('db', ['name' => 'db'], $values,
      ['return_record' => TRUE]);
    if ($db == FALSE) {
      print "ERROR: Unable to update db_table\n";
      exit;
    }
  }
  return $db_id;
}

/**
 * Updates the NCBI BioSample Attribute CV Terms
 *
 * Automatically downloads and imports into the CVterm table the attribute
 * names for biomaterials.
 */
function tripal_biomaterial_update_prop_terms() {
  // We use NCBI biosample attributes to fill the biomaterialprop cv table.
  // These attributes can be accessed at the following url.
  $url = "https://www.ncbi.nlm.nih.gov/biosample/docs/attributes/?format=xml";
  $xml_str = file_get_contents($url);
  $xml_obj = simplexml_load_string($xml_str);

  foreach ($xml_obj->Attribute as $attribute) {
    chado_insert_cvterm([
      'id' => 'NCBI_BioSample_Attributes:' . $attribute->Name,
      'name' => $attribute->Name,
      'definition' => $attribute->Description,
      'cv_name' => 'NCBI BioSample Attributes',
      'db_name' => 'NCBI_BioSample_Attributes',
    ], ['update_existing' => FALSE]);
  }
}
