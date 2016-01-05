<?php
/**
 * @file
 * Administration of Analysis: Expression module
 */

/**
 * Implements an admin page to congigure the display of analysis: expression data.
 * 
 * @ingroup tripal_analysis_expression
 */
function tripal_biomaterial_loader() {

  // Set the breadcrumbs. 
  $breadcrumb = array(); //*** update this for tripal
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Administration', 'admin');
  $breadcrumb[] = l('Tripal', 'admin/tripal');
  $breadcrumb[] = l('Extensions', 'admin/tripal/extension');
  $breadcrumb[] = l('Tripal Expression', 'admin/tripal/extension/tripal_analysis_expression');
  $breadcrumb[] = l('Analysis: Expression Biomaterial Loader', 'admin/tripal/extension/tripal_analysis_expression/biomaterial_loader');
  drupal_set_breadcrumb($breadcrumb);

  $organism_id  = '';

  $form['description'] = array(
    '#markup' => t('This loader will load the following fields if present. <br> 
     <b>biomaterial table</b>: name, description, organism, and biomaterial provider. <br> 
     <b>biomaterialprop table</b>: ncbi owner, age, collected by, cultivar, temperature, tissue, treatment, and geographic location. <br>
     <b>dbxref table</b>: sample accession, sample external id'),
  );

  $organisms = tripal_get_organism_select_options(FALSE);

  $form['organism_id'] = array(
    '#title' => t('Organism'),
    '#type' => 'select',
    '#required' => TRUE,
    '#description' => t('The organism from which the biosamples were collected.'),
    '#options' => $organisms,
    '#default_value' => $organism_id,
  );

  $form['file_type'] = array(
    '#title' => t('File type'),
    '#type' => 'radios',
    '#required' => TRUE,
    '#options' => array(
      'xml' => t('NCBI biosample xml file'),
      'flat' => t('Delineated flat file')
    ));

  $form['xml_file'] = array(
    '#type' => 'fieldset',
    '#title' => t('NCBI biosample xml file'),
    '#states' => array(
      'visible' => array(
        ':input[name="file_type"]' => array('value' => 'xml'),
      )
    )
  );

  $form['flat_file'] = array(
    '#type' => 'fieldset',
    '#title' => t('Delineated flat file'),
    '#states' => array(
      'visible' => array(
        ':input[name="file_type"]' => array('value' => 'flat'),
      )
    )
  );

  $form['xml_file']['xml_path'] = array(
    '#type' => 'textfield',
    '#title' => t('File Path'),
    '#description' => t('The path to the xml file.'),
    '#states' => array(
      'required' => array(
        ':input[name="file_type"]' => array('value' => 'xml'),
      )
    )
  );

  $form['xml_file']['xml_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Job'),
  );

  $form['flat_file']['flat_path'] = array(
    '#type' => 'textfield',
    '#title' => t('File Path'),
    '#description' => t('The path to the flat file.'),
    '#states' => array(
      'required' => array(
        ':input[name="file_type"]' => array('value' => 'flat'),
      )
    )
  );

  $form['flat_file']['flat_delimiter'] = array(
    '#type' => 'textfield',
    '#title' => t('Field Delimiter'),
    '#description' => t('Select a field delimiter such as "," or "\t". Delimiter must be a single character. Defaults to "\t".'),
    '#size' => 5,
  );

  $form['flat_file']['flat_enclosure'] = array(
    '#type' => 'textfield',
    '#title' => t('Field Enclosure'),
    '#description' => t('Select a field delimiter such as "\'" or """. Delimiter must be a single character. Leave blank if there is no enclosure character. Defaults to no delimiter.'),
    '#size' => 5,
  );

  $form['flat_file']['flat_regex'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field Regular Expressions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['flat_file']['flat_regex']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Unique Name'),
    '#description' => t('Regex to capture the unique name of the biomaterial. Case insensitive - defaults to "(library[\s|\_|\-]id|library|sample[\s|\_|\-]name|sample|name)".'),
  );

  $form['flat_file']['flat_regex']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Regex to capture the description of the biomaterial. Case insensitive - defaults to "(title\/short description|description)".'),
  );

  $form['flat_file']['flat_regex']['organism'] = array(
    '#type' => 'textfield',
    '#title' => t('Organism Name'),
    '#description' => t('Regex to capture name of the organism from which the biomaterial was collected. Case insensitive - defaults to "(organism)".'),
  );

  $form['flat_file']['flat_regex']['biomaterial_provider'] = array(
    '#type' => 'textfield',
    '#title' => t('Biomaterial Provider'),
    '#description' => t('Regex to capture the person or institution that provided the biomaterial. Case insensitive - defaults to "(biomaterial[\s|\_|\-]provider)".'),
  ); 

  $form['flat_file']['flat_regex']['ncbi_owner'] = array(
    '#type' => 'textfield',
    '#title' => t('NCBI owner'),
    '#description' => t('Regex to capture the NCBI owner of the biomaterial. Case insensitive - defaults to "(owner)".'),
  );

  $form['flat_file']['flat_regex']['age'] = array(
    '#type' => 'textfield',
    '#title' => t('Biomaterial Age'),
    '#description' => t('Regex to capture the age of the biomaterial. Case insensitive - defaults to "(age)".'),
  );

  $form['flat_file']['flat_regex']['collected_by'] = array(
    '#type' => 'textfield',
    '#title' => t('Collected By'),
    '#description' => t('Regex to capture the name of institution that collected the biomaterial. Case insensitive - defaults to "(collected[\s|\_|\-]by)".'),
  );

  $form['flat_file']['flat_regex']['cultivar'] = array(
    '#type' => 'textfield',
    '#title' => t('Cultivar'),
    '#description' => t('Regex to capture the biomaterial cultivar. Case insensitive - defaults to "(cultivar)".'),
  );

  $form['flat_file']['flat_regex']['temperature'] = array(
    '#type' => 'textfield',
    '#title' => t('Biomaterial Temperature'),
    '#description' => t('Regex to capture the temperature of the biomaterial. Case insensitive - defaults to "(temperature|temp)".'),
  );

  $form['flat_file']['flat_regex']['tissue'] = array(
    '#type' => 'textfield',
    '#title' => t('Biomaterial Tissue'),
    '#description' => t('Regex to capture the biomaterial tissue. Case insensitive - defaults to "(tissue)".'),
  );

  $form['flat_file']['flat_regex']['treatment'] = array(
    '#type' => 'textfield',
    '#title' => t('Unique Name'),
    '#description' => t('Regex to capture the treatment the biomaterial underwent. Case insensitive - defaults to "(treatment)".'),
  );

  $form['flat_file']['flat_regex']['geographic_location'] = array(
    '#type' => 'textfield',
    '#title' => t('Geographic Location'),
    '#description' => t('Regex to capture the geographic location where the biomaterial was collected. Case insensitive - defaults to "(geographic[\s|\_|\-]location)".'),
  );

  $form['flat_file']['flat_regex']['sample_accession'] = array(
    '#type' => 'textfield',
    '#title' => t('Sample Accession'),
    '#description' => t('Regex to capture the sample accession. Case insensitive - defaults to "(biosample[\s|\_|\-]accession|accession|external[\s|\_|\-]id)".'),
  );

  $form['flat_file']['flat_regex']['sample_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Sample Id'),
    '#description' => t('Regex to capture the sample id. Case insensitive - defaults to "(primary[\s|\_|\-]id)".'),
  );

  $form['flat_file']['flat_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Job'),
  );

  return $form;
}

function tripal_biomaterial_loader_submit($form, &$form_state) {

  $xml_path = $form_state['values']['xml_path'];
  $organism_id = $form_state['values']['organism_id'];

  $success = xml_biomaterial_parser($xml_path, $organism_id); 

}

function xml_biomaterial_parser($xml_path, $organism_id) {

  // Check file path to see if file is accessible.
  if (!is_readable($xml_path)) {
    print "ERROR: File is not readable or not present.";
    return 0;
  }

  // Load the contents of the xml file into a string.
  $xml_raw = file_get_contents($xml_path);

  if (!$xml_raw) {
    print "ERROR: Unable to read file.";
    return 0;
  }

  // Parse the file.
  $xml = simplexml_load_string($xml_raw);
 
  for ($i=0; $i<count($xml->BioSample); $i++) {
    add_xml_data($organism_id,$xml->BioSample[$i]);
  }
}

function add_xml_data($organism_id, $biosample) {

  // Extract data from the xml string. 
  $sample_accession = $biosample->Ids->Id[0];
  $unique_name = $biosample->Ids->Id[1];
  $sra_accession = $biosample->Ids->Id[2];
  $description = $biosample->Description->Title;
  $organism = $biosample->Description->Organism->OrganismName;
  $comment = $biosample->Description->Comment->Paragraph;
  $ncbi_owner = $biosample->Owner->Name;

  $biomaterial_provider = $biosample->Attributes->Attribute[1];
/*  $age = $biosample->Attributes->Attribute[0];
  $biomaterial_provider = $biosample->Attributes->Attribute[1];
  $collected_by = $biosample->Attributes->Attribute[2];
  $cultivar = $biosample->Attributes->Attribute[3];
  $temperature = $biosample->Attributes->Attribute[4];
  $tissue = $biosample->Attributes->Attribute[5];
  $treatment = $biosample->Attributes->Attribute[6];
  $geographic_location = $biosample->Attributes->Attribute[7]*/;
 // $version = preg_match('/last_update="(.+?)"/', $biosample);

  // Create a contact if contact is not present.
  $biosourceprovider_id = create_contact($biomaterial_provider);

  // Verify there is an NCBI biosample database.
  $ncbi_biosample_id = create_ncbi_db('bioproject', 'NCBI BioSample', '');    

  // Verify there is an NCBI sra database.
  $ncbi_sra_id = create_ncbi_db('sra', 'NCBI SRA', '');

  // Create entry in the dbxref table. 
  $dbxref_biosample= tripal_insert_dbxref(array(
    'db_id' => $ncbi_biosample_id,
    'accession' => $sample_accession,
   // 'version' => $version,
    'description' => '',
    )
  );
  $dbxref_biosample_id = $dbxref_biosample->dbxref_id;

  // If sra_accession is present, create entry in the dbxref table. 
  if ($sra_accession) {
    $dbxref_sra= tripal_insert_dbxref(array(
      'db_id' => $ncbi_sra_id,
      'accession' => $sra_accession,
      // 'version' => $version,
      'description' => '',
    ));
    $dbxref_sra_id = $dbxref_sra->dbxref_id;
  }

 // drupal_set_message((string)$biosample->Attributes->Attribute[0]);
 // drupal_set_message((string)$unique_name);

  // Create a biomaterial.
  $biomaterial_id = create_biomaterial($unique_name, $comment, $organism_id, $biosourceprovider_id, $dbxref_biosample_id);
 // drupal_set_message($biomaterial_id);

  add_biomaterial_properties($biosample->Attributes->Attribute, $biomaterial_id);
  // Add to biomaterialprop table.
//  add_biomaterial_properties($biomaterial_id, $sra_accession, $description, $ncbi_owner, $age, $collected_by, $cultivar, $temperature, $tissue, $treatment, $geographic_location);

  // Add to biomaterial_dbxref table.
  if ($sra_accession) {
    tripal_associate_dbxref('biomaterial', $biomaterial_id, array('accession' => $sra_accession, 'db_name' => 'NCBI SRA'));
  }
}


function add_biomaterial_properties($attributes, $biomaterial_id) {

  $record = array(
    'table' => 'biomaterial',
    'id' => $biomaterial_id,
  );

  $options = array(
    'update_if_present' => TRUE,
  );

  // Iterate through each property
  foreach($attributes as $attr) {
    drupal_set_message("here" . var_export($attr,TRUE));
    // Get the cvterm name 
    $attr_name = (string)$attr->attributes()->attribute_name;
    drupal_set_message(var_export($attr_name,TRUE));
    drupal_set_message((string)$attr_name);
    // Insert into database. If the type is already in the database the function will not update. 
    if(!tripal_insert_cvterm(array(
      'name' => (string)$attr_name, 
      'definition' => '',
      'cv_name' => 'biomaterial_property', 
      'is_relationship' => 0,
      'db_name' => 'tripal'), 
      array('update_existing' => FALSE))) {
      drupal_set_message("ERRORR");
      print "ERROR: Could not insert biomaterial attribute name into the controlled vocabulary table.";
      return 0;
    } 
  
    // Insert the property into the biomaterialprop table.  
    $property = array('type_name' => $attr_name, 'cv_name' => 'biomaterial_property', 'value' => $attr); 
    chado_insert_property($record, $property, $options);

  }

}

function create_contact($contact_name) {
   $contact_id = chado_query('select contact_id from {contact} where name = :name limit 1',array(':name' => $contact_name))->fetchField();
  if(!$contact_id) {
    $values = array(
      'name' => $contact_name,
    );
    $contact = chado_insert_record('contact', $values);
    if($contact==FALSE) {
      print "Unable to write to contact table\n";
      return 0;
    }
    $contact_id = $contact['contact_id'];
  }
  return $contact_id;
}

function create_ncbi_db($url_name, $db_name, $db_description) {
  
  $url = "http://www.ncbi.nlm.nih.gov/";
  $urlprefix = "http://www.ncbi.nlm.nih.gov/" . $url_name . "/";

  $values = array(
    'name' => $db_name,
    'description' => $db_description,
    'urlprefix' => $urlprefix,
    'url' => $url,
  );

  $db_id = chado_query('select db_id from {db} where name = :name limit 1',array(':name' => $db_name))->fetchField();

  if(!$db_id) {
    $db = chado_insert_record('db', $values);
    if($db==FALSE) {
      print "Unable to write to db table\n";
      return 0;
    }
    $db_id = $db['db_id'];
  }

  if($db_id) {
    $db = chado_update_record('db',
      array('name' => 'db'),
      $values,
      array('return_record' => TRUE));
    if($db==FALSE) {
      print "Unable to update db_table\n";
      return 0;
    }
  }

  return $db_id;
}




/**
 * Implements a function that validates and submits the form that configures the display of analysis: expression data.
 * 
 * @ingroup tripal_analysis_expression
 */
function tripal_biomaterial_loader_validate($form, &$form_state) {
}

