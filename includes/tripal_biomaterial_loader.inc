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
function tripal_biomaterial_loader($form, &$form_state) {

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

  $form['#tree'] = TRUE;

  $form['description'] = array(
    '#markup' => t('This loader will load the following tables if corresponding fields are present. <br> 
     <b>biomaterial table</b>: name, description, organism, and biomaterial provider. <br> 
     <b>biomaterialprop table</b>: Other fields such as cultivar, temperature, tissue, and treatment can be loaded into the properties table. <br>
     <b>dbxref table</b>: Accession values can be loaded into the database reference table.'),
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

  $form['flat_file']['regex_description'] = array(
    '#markup' => t('The flat file loader expects a file where the first line is the title of each column. 
    The only field that is required to create a biomaterial is the name of the biomaterial. 
    It is recommended that a description, organism, biomaterial provider, accession, treatment, and tissue
    also be provided. A biomaterialprop cvterm type will be created for column title not already present
    in the database.')
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
    '#description' => t('Select a field delimiter such as "," or "\t". Delimiter must be a single character (an escaped character is considered a single character). Defaults to "\t".'),
    '#size' => 5,
  );

  $form['flat_file']['flat_enclosure'] = array(
    '#type' => 'textfield',
    '#title' => t('Field Enclosure'),
    '#description' => t('Select a field delimiter such as "\'" or """. Delimiter must be a single character (an escaped character is considered a single character). Leave blank if there is no enclosure character. Defaults to no delimiter.'),
    '#size' => 5,
  );

  $form['flat_file']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Regex to capture the unique name of the biomaterial. Case insensitive - defaults to "(library[\s|\_|\-]id|library|sample[\s|\_|\-]name|sample|name)".'),
  );

  $form['flat_file']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#description' => t('Regex to capture the description of the biomaterial. Case insensitive - defaults to "(title|short description|description)".'),
  );

  $form['flat_file']['sample_accession'] = array(
    '#type' => 'textfield',
    '#title' => t('Sample Accession'),
    '#description' => t('Regex to capture the sample accession. Case insensitive - defaults to "(biosample[\s|\_|\-]accession|accession|external[\s|\_|\-]id)".'),
  );

  $form['flat_file']['biomaterial_provider'] = array(
    '#type' => 'textfield',
    '#title' => t('Biomaterial Provider'),
    '#description' => t('Regex to capture the person or institution that provided the biomaterial. 
    Case insensitive - defaults to "(biomaterial[\s|\_|\-]provider)". If not present in the 
    database a contact will be created with the name of the biomaterial provider.'),
  ); 

  $form['flat_file']['fields_description'] = array(
    '#markup' => t('Other fields in the flat file can be loaded as biomaterial properties (biomaterialprop table) or as database accessions (biomaterial_dbxref table).'),
  );

  $form['flat_file']['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Biomaterial Properties'),
    '#prefix' => '<div id="biomaterial-properties-wrapper">',
    '#suffix' => '</div>',
  );

  $form['flat_file']['dbxrefs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Biomaterial Database Accessions'),
    '#prefix' => '<div id="biomaterial-dbxrefs-wrapper">',
    '#suffix' => '</div>',
  );

  if (empty($form_state['flat_file']['properties']['biomaterial_properties']['num_properties'])) {
    $form_state['flat_file']['properties']['biomaterial_properties']['num_properties'] = 1;
  }

  for ($i=0;$i<$form_state['flat_file']['properties']['biomaterial_properties']['num_properties'];$i++) {

    $form['flat_file']['properties']['biomaterial_properties'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t('Property ' . ($i+1)), 
    );

    $form['flat_file']['properties']['biomaterial_properties'][$i]['biomaterial_prop_regex'] = array(
      '#type' => 'textfield',
      '#title' => t('Regex'),
      '#prefix' => '<div class="container-inline">',
      '#suffix' => '</div><br>',
      '#description' => t('Regex to capture the property from the flat file.'),
    ); 

    $form['flat_file']['properties']['biomaterial_properties'][$i]['biomaterial_prop_type'] = array(
      '#type' => 'textfield',
      '#title' => t('Cvterm'),
      '#prefix' => '<div class="container-inline">',
      '#suffix' => '</div><br>',
      '#description' => t('Name of the biomaterial property type to be inserted into the cvterm table.'),
    );

  }

  if (empty($form_state['flat_file']['dbxrefs']['biomaterial_dbxrefs']['num_dbxrefs'])) {
    $form_state['flat_file']['dbxrefs']['biomaterial_dbxrefs']['num_dbxrefs'] = 1;
  }

  //$sql = "SELECT name FROM db WHERE





  for ($i=0;$i<$form_state['flat_file']['dbxrefs']['biomaterial_dbxrefs']['num_dbxrefs'];$i++) {

    $form['flat_file']['dbxrefs']['biomaterial_dbxrefs'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t('Database Accession ' . ($i+1)), 
    );

    $form['flat_file']['dbxrefs']['biomaterial_dbxrefs'][$i]['biomaterial_dbxref_regex'] = array(
      '#type' => 'textfield',
      '#title' => t('Regex'),
      '#prefix' => '<div class="container-inline">',
      '#suffix' => '</div><br>',
      '#description' => t('Regex to capture the database accession from the flat file.'),
    ); 

    $form['flat_file']['dbxrefs']['biomaterial_dbxrefs'][$i]['biomaterial_dbxref_type'] = array(
      '#type' => 'select',
      '#title' => t('Select database'),
   //   '#prefix' => '<div class="container-inline">',
    //  '#suffix' => '</div><br>',
      '#size' => 6,
    );
  /*  $form['flat_file']['dbxrefs']['biomaterial_dbxrefs'][$i]['biomaterial_dbxref_type'] = array(
      '#type' => 'textfield',
      '#title' => t('Cvterm'),
      '#prefix' => '<div class="container-inline">',
      '#suffix' => '</div><br>',
      '#description' => t('Name of the biomaterial database accession type to be inserted into the cvterm table.'),
    ); */

  }

  $form['flat_file']['properties']['biomaterial_properties']['add_property'] = array(
    '#type' => 'submit',
    '#value' => t('Add Property'),
    '#submit' => array('ajax_add_biomaterial_property_submit'),
    '#ajax' => array(
      'callback' => 'ajax_add_biomaterial_property_callback',
      'wrapper' => 'biomaterial-properties-wrapper',
    ),
  );

  if ($form_state['flat_file']['properties']['biomaterial_properties']['num_properties'] > 1) {
    $form['flat_file']['properties']['biomaterial_properties']['remove_property'] = array(
      '#type' => 'submit',
      '#value' => t('Remove Property'),
      '#submit' => array('ajax_remove_biomaterial_property_submit'),
      '#ajax' => array(
        'callback' => 'ajax_add_biomaterial_property_callback',
        'wrapper' => 'biomaterial-properties-wrapper',
      ),
    );
  }

  $form['flat_file']['dbxrefs']['biomaterial_dbxrefs']['add_dbxref'] = array(
    '#type' => 'submit',
    '#value' => t('Add Database Accession'),
    '#submit' => array('ajax_add_biomaterial_dbxref_submit'),
    '#ajax' => array(
      'callback' => 'ajax_add_biomaterial_dbxref_callback',
      'wrapper' => 'biomaterial-dbxrefs-wrapper',
    ),
  );

  if ($form_state['flat_file']['dbxrefs']['biomaterial_dbxrefs']['num_dbxrefs'] > 1) {
    $form['flat_file']['dbxrefs']['biomaterial_dbxrefs']['remove_dbxref'] = array(
      '#type' => 'submit',
      '#value' => t('Remove Database Accession'),
      '#submit' => array('ajax_remove_biomaterial_dbxref_submit'),
      '#ajax' => array(
        'callback' => 'ajax_add_biomaterial_dbxref_callback',
        'wrapper' => 'biomaterial-dbxrefs-wrapper',
      ),
    );
  }

  $form['flat_file']['flat_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Job'),
  );

  return $form;
}

function ajax_add_biomaterial_property_callback($form, $form_state) {
  return $form['flat_file']['properties'];
}

function ajax_add_biomaterial_property_submit($form, &$form_state) {
  $form_state['flat_file']['properties']['biomaterial_properties']['num_properties']++;
  $form_state['rebuild'] = TRUE;
}

function ajax_remove_biomaterial_property_submit($form, &$form_state) {
  if ($form_state['flat_file']['properties']['biomaterial_properties']['num_properties'] > 1) {
    $form_state['flat_file']['properties']['biomaterial_properties']['num_properties']--;
  }
  $form_state['rebuild'] = TRUE;
}

function ajax_add_biomaterial_dbxref_callback($form, $form_state) {
  return $form['flat_file']['dbxrefs'];
}

function ajax_add_biomaterial_dbxref_submit($form, &$form_state) {
  $form_state['flat_file']['dbxrefs']['biomaterial_dbxrefs']['num_dbxrefs']++;
  $form_state['rebuild'] = TRUE;
}

function ajax_remove_biomaterial_dbxref_submit($form, &$form_state) {
  if ($form_state['flat_file']['dbxrefs']['biomaterial_dbxrefs']['num_dbxrefs'] > 1) {
    $form_state['flat_file']['dbxrefs']['biomaterial_dbxrefs']['num_dbxrefs']--;
  }
  $form_state['rebuild'] = TRUE;
}

function tripal_biomaterial_loader_submit($form, &$form_state) {

  $xml_path = $form_state['values']['xml_path'];
  $flat_path = $form_state['values']['flat_path'];
  $organism_id = $form_state['values']['organism_id'];
  $file_type = $form_state['values']['file_type'];
  $flat_delimiter = $form_state['values']['flat_delimiter'];
  $flat_enclosure = $form_state['values']['flat_enclosure'];
  $regex['name'] = $form_state['values']['name'];
  $regex['description']= $form_state['values']['description'];
 // $regex[] = $form_state['values']['organism'];
  $regex['dbxref'] = $form_state['values']['sample_accession'];
  $regex['biomaterial_provider'] = $form_state['values']['biomaterial_provider'];
  $regex['tissue']= $form_state['values']['tissue'];
  $regex['treatment'] = $form_state['values']['treatment'];

  if ($file_type == 'xml') {
    $success = xml_biomaterial_parser($xml_path, $organism_id); 
  }

  if ($file_type == 'flat') {
    $success = flat_biomaterial_parser($flat_path, $organism_id, $file_type, $flat_delimiter,
                                       $flat_enclosure, $regex);
  }
}


function flat_biomaterial_parser($flat_path, $organism_id, $file_type, $flat_delimiter, $flat_enclosure, $regex) {

  // Check file path to see if file is accessible.
  if (!is_readable($flat_path)) {
    print "ERROR: File is not readable or not present.";
    return 0;
  }

  // Get the file pointer.
  $fp = fopen($flat_path, "r");

  $headers = fgetcsv($fp);

  $key_values = [];
  $tissue_key = '';
  $treatment_key = '';

  for ($i=0;$i<count($headers);$i++) {
    if(preg_match($regex['name'], $headers[$i])) {
      $key_values['name'] = $i; 
    }  
    if(preg_match($regex['description'], $headers[$i])) {
      $key_values['description'] = $i; 
    }  
    if(preg_match($regex['dbxref'], $headers[$i])) {
      $key_values['dbxref'] = $i; 
    }  
    if(preg_match($regex['biomaterial_provider'], $headers[$i])) {
      $key_values['biomaterial_provider'] = $i; 
    }  
    if(preg_match($regex['tissue'], $headers[$i])) {
      $tissue_key = $i; 
    } 
    if(preg_match($regex['treatment'], $headers[$i])) {
      $treatment_key = $i; 
    }  
  }

  while($line = fgetcsv($fp)) {

    $values = [];
    $values['organism_id'] = $organism_id;

    if (array_key_exists('name', $treatment_key)) {
      $values['name'] = $name;
    } 

    if (array_key_exists('description', $treatment_key)) {
      $values['description'] = $description;
    } 

    if (array_key_exists('biomaterial_provider', $treatment_key)) {
      $biosourceprovider_id = create_contact($line[$treatment_key['biomaterial_provider']]);
      $values['biosourceprovider_id'] = $biosourceprovider_id;
    } 

  }

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

  if ($biomaterial_provider) {
    $biosourceprovider_id = create_contact($biomaterial_provider);
  }
  else {
    $biosourceprovider_id = '';
  }
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
  //$biomaterial_id = create_biomaterial($unique_name, $comment, $organism_id, $biosourceprovider_id, $dbxref_biosample_id, $description);

  $biomaterial_description = $comment ? (string)$comment : (string)$description;

 drupal_set_message($biomaterial_description);
  $biomaterial_id = create_biomaterial($unique_name, '', $organism_id, $biosourceprovider_id, $dbxref_biosample_id, $biomaterial_description);
 // drupal_set_message($biomaterial_id);

//  drupal_set_message($description);

  add_xml_biomaterial_properties($biosample->Attributes->Attribute, $biomaterial_id);
  // Add to biomaterialprop table.
//  add_biomaterial_properties($biomaterial_id, $sra_accession, $description, $ncbi_owner, $age, $collected_by, $cultivar, $temperature, $tissue, $treatment, $geographic_location);

  // Add to biomaterial_dbxref table.
  if ($sra_accession) {
    tripal_associate_dbxref('biomaterial', $biomaterial_id, array('accession' => $sra_accession, 'db_name' => 'NCBI SRA'));
  }
}


function add_xml_biomaterial_properties($attributes, $biomaterial_id) {

  $record = array(
    'table' => 'biomaterial',
    'id' => $biomaterial_id,
  );

  $options = array(
    'update_if_present' => TRUE,
  );

  // Iterate through each property
  foreach($attributes as $attr) {
   // drupal_set_message("here" . var_export($attr,TRUE));
    // Get the cvterm name 
    $attr_name = (string)$attr->attributes()->attribute_name;
    //drupal_set_message(var_export($attr_name,TRUE));
    //drupal_set_message((string)$attr_name);
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

