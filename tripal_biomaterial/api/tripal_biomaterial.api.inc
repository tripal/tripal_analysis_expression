<?php
/**
 *
 * @file
 * Functions to interact with biomaterials.
 *
 * @ingroup tripal_biomaterial
 */

/**
 *
 * Implements a function to create a generic quantification. Only a name,
 * acuqisition_id, and analysis_id are required to create an assay record. A
 * biomaterial_id, organism_id, and analysis_id are provided to the function to
 * create a unique quantification name.
 *
 * @param $acquisition
 *  The acquisition_id of the acquisition with which the quantification is
 *   associated. This parameter is necessary for the creation of the
 *   quantification record.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis associated with the experiment. This
 *   parameter is necessary for the creation of the quantification record. This
 *   record will also be used to create a unique name for the quantification.
 *
 * @ingroup tripal_analysis_expression
 */

function create_generic_quantification($acquisition_id, $analysis_id, $uniq_name) {

  $quantification_id = chado_query('select quantification_id from {quantification} where name = :name limit 1',
    [':name' => $uniq_name])->fetchField();
  if (!$quantification_id) {
    $values = [
      'name' => $uniq_name,
      'acquisition_id' => $acquisition_id,
      'analysis_id' => $analysis_id,
    ];
    $quantification = chado_insert_record('quantification', $values);
    if ($quantification == FALSE) {
      print "ERROR: Unable to write to quantification table\n";
      exit;
    }
    $quantification_id = $quantification['quantification_id'];
  }
  return $quantification_id;
}

/**
 *
 * Implements a function that inserts a record into the element table.
 *
 * @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment. This
 *   parameter is necessary for the creation of the element record.
 *
 * @param $feature_id
 *  The feature_id of the feature associated with the element. This parameter
 *   is necessary for the creation of the element record.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_element($arraydesign_id, $feature_id) {
  if (empty($feature_id) || !$feature_id) {
    tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
      'Please provide a valid feature id');
    return NULL;
  }

  $element_id = chado_query('SELECT element_id FROM {element} WHERE arraydesign_id = :arraydesign_id AND feature_id = :feature_id',
    [
      ':arraydesign_id' => $arraydesign_id,
      ':feature_id' => $feature_id,
    ])->fetchField();
  if (!$element_id) {
    $values = [
      'arraydesign_id' => $arraydesign_id,
      'feature_id' => $feature_id,
    ];
    $element = chado_insert_record('element', $values);
    if ($element == FALSE) {
      tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR,
        "ERROR: Unable to write to element table\n");
      return NULL;
    }
    $element_id = $element['element_id'];
  }
  return $element_id;
}

/**
 *
 * Implements a funtion that inserts a record into the elementresult table.
 *
 * @param $element_id
 *  The element_id of the element associated with the elementresult. This
 *   parameter is necessary for the creation of the elementresult record.
 *
 * @param $quantification_id
 *  The quantification_id of the quantification associated with the experiment.
 *   This parameter is necessary for the creation of the elementresult record.
 *
 * @ingroup tripal_analysis_expression
 */

function tripal_expression_elementresult($element_id, $quantification_id, $signal) {
  if (empty($element_id) || !$element_id) {
    return NULL;
  }

  $values = [
    'element_id' => $element_id,
    'quantification_id' => $quantification_id,
    'signal' => $signal,
  ];

  $elementresult_id = chado_query('SELECT elementresult_id FROM {elementresult} WHERE element_id = :element_id AND quantification_id = :quantification_id',
    [
      ':element_id' => $element_id,
      ':quantification_id' => $quantification_id,
    ])->fetchField();
  if (!$elementresult_id) {
    $elementresult = chado_insert_record('elementresult', $values);
    if ($elementresult == FALSE) {
      print "ERROR: Unable to write to elementresult table\n";
      return NULL;
    }
  }
  if ($elementresult_id) {
    $elementresult = chado_update_record('elementresult', [
      'element_id' => $element_id,
      'quantification_id' => $quantification_id,
    ], $values);
    if ($elementresult == FALSE) {
      print "ERROR: Unable to update elementresult table\n";
      return NULL;
    }
  }

  $elementresult_id = $elementresult['elementresult_id'];
  return $elementresult_id;
}

/**
 *
 * Implements a function to create records in the following tables:
 * biomaterial,
 *  contact, assay, acquisition, quantification, channel, and
 * assay_biomaterial.
 *  The function will return a quantification_id that can be associated with
 *  features through the element and elementresult tables.
 *
 * @param $biomaterial
 *  The name of a biomaterial. This name will be used to create a new
 *   biomaterial record.
 *
 * @param $organism_id
 *  The organism_id of the organism from which the biomaterial was gathered.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis: expression from which the expression
 *   loader is being called.
 *
 * @param $contact_id
 *  The contact_id of the contact that provided the biomaterial. This paramter
 *   is optional. If the parameter is not provided, a generic contact record
 *   will be created in the contact table.
 *
 * @param $arraydesign_id
 *  The arraydesign_id of the arraydesign associated with the experiment.
 *
 * @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is
 *   optional. If the parameter is not provided, a generic assay record will be
 *   created in the assay table.
 *
 * @param $acquisition_id
 *  The acquisition_id of the acquisition associated with the experiment. This
 *   parameter is optional. If the parameter is not provided, a generic
 *   acquisition record will be created in the acquisition table.
 *
 * @param $quantificatin_id
 *  The quantification_id of the quantification associated with the experiment.
 *   This parameter is optional. If the parameter is not provided, a generic
 *   quantification record will be created in the quantification table.
 *
 * @param  $quantificationunits
 * The units associated with the quantification.  Optional.
 *
 * @ingroup tripal_analysis_expression
 */

function expression_create_biomaterial_structure($biomaterial, $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $quantificationunits, $uniq_name, $warning) {
  // Create the biomaterial
  $biomaterial_id = tripal_biomaterial_create_biomaterial($biomaterial,
    $analysis_id, $organism_id, $contact_id, $dbxref_id = NULL, '', $warning);

  $uniq_name = $biomaterial . $uniq_name;

  // Create the following records if not provided by the user.
  $contact_id = $contact_id ? $contact_id : create_generic_contact();
  $assay_id = $assay_id ? $assay_id : create_generic_assay($contact_id,
    $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
  $acquisition_id = $acquisition_id ? $acquisition_id : create_generic_acquisition($assay_id,
    $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
  $quantification_id = $quantification_id ? $quantification_id : create_generic_quantification($acquisition_id,
    $analysis_id, $uniq_name);

  // Create a generic channel. A channel_id is necessary for the assay_biomaterial table.
  $channel_id = create_generic_channel();
  $assay_biomaterial_id = create_assay_biomaterial($assay_id, $biomaterial_id,
    $channel_id);

  //Attach the units to the quantification
  if ($quantificationunits) {

    $cvterm_id = tripal_get_cvterm([
      'name' => 'unit_of_measure',
      'cv_id' => [
        'name' => 'sep',
      ],
    ])->cvterm_id;

    if (!$cvterm_id) {
      $cvterm_id = tripal_insert_cvterm([
        'id' => 'sep:00056',
        'name' => 'unit_of_measure',
        'cv_name' => 'sep',
        'definition' => 'A unit of measure is a quantity which is a standard of measurement for some dimension. For example, the Meter is a Unit Of Measure for the dimension of length, as is the Inch. There is no intrinsic property of a UnitOfMeasure that makes it primitive or fundamental; rather, a system of units (e.g. Systeme International Unit) defines a set of orthogonal dimensions and assigns units for each. [ SUMO:unit of measure ]',
      ])->cvterm_id;
    }

    $values = [
      'quantification_id' => $quantification_id,
      'type_id' => $cvterm_id,
      'value' => $quantificationunits,
    ];
    $result = chado_insert_record('quantificationprop', $values);
  }
  return $quantification_id;
}

/**
 *
 * Implements a function to create a record in the assay_biomaterial table. The
 * assay_biomaterial table connects the assay and biomaterial table. As well as
 * an assay and biomaterial record, the assay_biomaterial table also requires a
 * channel record.
 *
 * @param $assay_id
 *  The assay_id of the assay associated with the experiment. This parameter is
 *   necessary for the creation of an assay_biomaterial record.
 *
 * @param $biomaterial_id
 *  The biomaterial_id of one of the biomaterials in the experiment. This
 *   parameter is necessary for the creation of the assay_biomaterial record.
 *
 * @param $channel_id
 *  The channel_id of the channel of the assay containing the biomaterial. This
 *   parameter is necessary for the creation of the assay_biomaterial record.
 *
 * @ingroup tripal_analysis_expression
 */
function create_assay_biomaterial($assay_id, $biomaterial_id, $channel_id) {
  $assay_biomaterial_id = chado_query('select assay_biomaterial_id from {assay_biomaterial} where rank = :rank and assay_id = :assay_id and biomaterial_id = :biomaterial_id and channel_id = :channel_id limit 1',
    [
      ':rank' => 1,
      ':assay_id' => $assay_id,
      ':biomaterial_id' => $biomaterial_id,
      ':channel_id' => $channel_id,
    ])->fetchField();
  if (!$assay_biomaterial_id) {
    $values = [
      'rank' => 1,
      'assay_id' => $assay_id,
      'biomaterial_id' => $biomaterial_id,
      'channel_id' => $channel_id,
    ];
    $assay_biomaterial = chado_insert_record('assay_biomaterial', $values);
    if ($assay_biomaterial == FALSE) {
      print "ERROR: Unable to write to assay_biomaterial table\n";
      exit;
    }
    $assay_biomaterial_id = $assay_biomaterial['assay_biomaterial_id'];
  }
  return $assay_biomaterial_id;
}

/**
 *
 * Implements a function to create a generic contact.
 *
 * @ingroup tripal_analysis_expression
 */

function create_generic_contact() {
  $contact_id = chado_query('select contact_id from {contact} where name = :name limit 1',
    [':name' => 'Not provided'])->fetchField();
  if (!$contact_id) {
    $values = [
      'name' => 'Not provided',
      'description' => 'Caution: This is a generic contact created by the expression module. This contact may be a dependency for an expression analysis. Delete with caution.',
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

/**
 *
 * Implements a function to create a generic channel.
 *
 * @ingroup tripal_analysis_expression
 */

function create_generic_channel() {
  $channel_id = chado_query('select channel_id from {channel} where name = :name limit 1',
    [':name' => 'Not provided'])->fetchField();
  if (!$channel_id) {
    $values = [
      'name' => 'Not provided',
      'definition' => 'Caution: This is a generic channel created by the expression module. This channel may be a dependency for an expression analysis. Delete with caution.',
    ];
    $channel = chado_insert_record('channel', $values);
    if ($channel == FALSE) {
      print "ERROR: Unable to write to channel table\n";
      exit;
    }
    $channel_id = $channel['channel_id'];
  }
  return $channel_id;
}

/**
 *
 * Implements a function to create a generic arraydesign.
 *
 * @ingroup tripal_analysis_expression
 */

function create_generic_arraydesign($contact_id) {
  $arraydesign_id = chado_query('select arraydesign_id from {arraydesign} where name = :name limit 1',
    [':name' => 'Not provided'])->fetchField();
  if (!$arraydesign_id) {
    $values = [
      'name' => 'Not provided',
      'description' => 'Caution: This is a generic arraydesign created by the expression module. This arraydesign may be a dependency for an expression analysis. Delete with caution.',
      'manufacturer_id' => $contact_id,
      'platformtype_id' => 1,
    ];
    $arraydesign = chado_insert_record('arraydesign', $values);
    if ($arraydesign == FALSE) {
      print "ERROR: Unable to write to arraydesign table\n";
      exit;
    }
    $arraydesign_id = $arraydesign['arraydesign_id'];
  }
  return $arraydesign_id;
}

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
 * /**
 *
 * Implements a function to create a generic assay. Only a name, an
 * arraydesign_id, and a contact_id are required to create an assay record. A
 * biomaterial_id, organism_id, and analysis_id are provided to the function to
 * create a unique assay name.
 *
 * @param $contact_id
 *  The contact_id of the operator associated with the assay. In the case of
 *   this function this contact_id will belong to a generic contact record.
 *   This parameter is necessary for the creation of the assay record.
 *
 * @param $arraydesign
 *  The arraydesign_id of the array design associated with the assay. This
 *   parameter is necessary for the creation of the assay record.
 *
 * @param $biomaterial_id
 *  The biomaterial_id of one of the biomaterials of the experiment. This will
 *   be used to create a unique name for the assay.
 *
 * @param $organism_id
 *  The organism_id of the organism associated with the experiment. This will
 *   be used to create a unique name for the assay.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis associated with the experiment. This will
 *   be used to create a unique name for the assay.
 *
 * @ingroup tripal_analysis_expression
 */

function create_generic_assay($contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name) {
  /*$biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
  $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
  $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField(); */
  $assay_id = chado_query('select assay_id from {assay} where name = :name limit 1',
    [':name' => $uniq_name])->fetchField();
  //$assay_id = chado_query('select assay_id from {assay} where name = :name limit 1',array(':name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis))->fetchField();
  if (!$assay_id) {
    $values = [
      'name' => $uniq_name,
      //'name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
      //'description' => 'This is a generic assay created for the biomaterial: '. $biomaterial . '. The biomaterial was taken from the organism: ' . $organism_g . ' ' . $organism_s . '. This assay is part of the analysis: ' . $analysis,
      'arraydesign_id' => $arraydesign_id,
      'operator_id' => $contact_id,
      ####****
    ];
    $assay = chado_insert_record('assay', $values);
    if ($assay == FALSE) {
      print "ERROR: Unable to write to assay table\n";
      exit;
    }
    $assay_id = $assay['assay_id'];
  }
  return $assay_id;
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
 *
 * Implements a function to create a generic acquisition. Only a name and
 * assay_id, are required to create an assay record. A biomaterial_id,
 * organism_id, and analysis_id are provided to the function to create a unique
 * acquisition name.
 *
 * @param $assay
 *  The assay_id of the assay with which the acquisition is associated.
 *  This parameter is necessary for the creation of the acquisition record.
 *
 * @param $biomaterial_id
 *  The biomaterial_id of one of the biomaterials of the experiment. This will
 *   be used to create a unique name for the acquisition.
 *
 * @param $organism_id
 *  The organism_id of the organism associated with the experiment. This will
 *   be used to create a unique name for the acquisition.
 *
 * @param $analysis_id
 *  The analysis_id of the analysis associated with the experiment. This will
 *   be used to create a unique name for the acquisition.
 *
 * @ingroup tripal_analysis_expression
 */

function create_generic_acquisition($assay_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name) {

  $acquisition_id = chado_query('select acquisition_id from {acquisition} where name = :name limit 1',
    [':name' => $uniq_name])->fetchField();

  if (!$acquisition_id) {
    $values = [
      'name' => $uniq_name,
      'assay_id' => $assay_id,
    ];
    $acquisition = chado_insert_record('acquisition', $values);
    if ($acquisition == FALSE) {
      print "ERROR: Unable to write to acquisition table\n";
      exit;
    }
    $acquisition_id = $acquisition['acquisition_id'];
  }
  return $acquisition_id;
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
 * Updates the NCBI Biosample Attribute CV Terms
 *
 * Automatically downloads and imports into the CVterm table the attribute
 * names for biomaterials.
 */
function tripal_biomaterial_update_prop_terms() {
  // We use NCBI biosample attributes as to fill the biomaterialprop cv table.
  // These attributes can be accessed at the following url.
  $url = "http://www.ncbi.nlm.nih.gov/biosample/docs/attributes/?format=xml";
  $xml_str = file_get_contents($url);
  $xml_obj = simplexml_load_string($xml_str);

  foreach ($xml_obj->Attribute as $attribute) {
    chado_insert_cvterm([
      'id' => 'NCBI_BioSample_Terms:' . $attribute->Name,
      'name' => $attribute->Name,
      'definition' => $attribute->Description,
      'cv_name' => 'NCBI Biosample Attributes',
      'db_name' => 'NCBI_BioSample_Terms',
    ], ['update_existing' => FALSE]);
  }
}
