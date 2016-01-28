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
    '#markup' => t('This loader is designed to load data from NCBI BioSample compatible formats.
      If you have loaded data into NCBI, download download an xml file from NCBI with your 
      BioSamples. This will ensure biomaterials have the BioSample accession number. 
      If you have not uploaded to NCBI, load a flatfile in the NCBI BioSample Submission format. 
      The NCBI BioSample submission format can be downloaded 
      <a href="https://submit.ncbi.nlm.nih.gov/biosample/template/" target = "_blank">here</a>. 
      Available attributes to use can be found 
      <a href="http://www.ncbi.nlm.nih.gov/biosample/docs/attributes/" target = "_blank">here</a>. 
      You may add additional fields to the file, by adding another column to the submission file. 
      The header of the column will be used as the property type. <b>Biomaterials must have a unique
      sample_name</b>')
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
    '#default_value' => '\t',
  );

  $form['flat_file']['flat_enclosure'] = array(
    '#type' => 'textfield',
    '#title' => t('Field Enclosure'),
    '#description' => t('Select a field delimiter such as "\'" or """. Delimiter must be a single character (an escaped character is considered a single character). Leave blank if there is no enclosure character. Defaults to no delimiter.'),
    '#size' => 5,
    '#default_value' => '',
  );

  $form['flat_file']['flat_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Job'),
  );

  return $form;
}

function tripal_biomaterial_loader_submit($form, &$form_state) {
//**
  $primary_db_id = $form_state['values']['flat_file']['primary_db_id']; 

  // Get the file path. 
  $file_type = $form_state['values']['file_type'];
  if ($file_type == 'xml') {
    $path = $form_state['values']['xml_file']['xml_path'];
  }
  else {
    $path = $form_state['values']['flat_file']['flat_path'];
  }

  $organism_id = $form_state['values']['organism_id'];
  $flat_delimiter = $form_state['values']['flat_file']['flat_delimiter'];
  $flat_enclosure = $form_state['values']['flat_file']['flat_enclosure'];


  if ($file_type == 'xml') {
    $success = xml_biomaterial_parser($path, $organism_id); 
  }

  if ($file_type == 'flat') {
    $success = flat_biomaterial_parser($path, $organism_id, $file_type, $flat_delimiter, $flat_enclosure);
  } 
}


function flat_biomaterial_parser($flat_path, $organism_id, $file_type, $flat_delimiter, $flat_enclosure) {

  // Check file path to see if file is accessible.
  if (!is_readable($flat_path)) {
    print "ERROR: File is not readable or not present.";
    return 0;
  }

  // Get the file pointer.
  $fp = fopen($flat_path, "r");

  $headers = fgetcsv($fp);

  if(count(array_unique($headers))<count($headers)) {
    print "ERROR: Two of the column headers in the BioSample submission file have the same value\n";
    print "Each header must be a unique value.";
    exit(1);
  }

  $hit = 0;

  $biomaterial_names = [];

  $biomaterial_header = [];
  $prop_headers = [];
  $acc_headers = [];


  for ($i=0; $i < count($headers); $i++) {
    if ($header[$i] == 'sample_name') {
      $biomaterial_header['sample_name'] = $i;
    }
    if ($header[$i] == 'organism') {
      $biomaterial_header['organism'] = $i;
    }
    if ($header[$i] == 'description') {
      $biomaterial_header['description'] = $i;
    }
    if ($header[$i] == 'biomaterial_provider') {
      $biomaterial_header['biomaterial_provider'] = $i;
    }
    if ($header[$i] == 'bioproject_accession') {
      $acc_header['bioproject_accession'] = $i;
    }
    else {
      $prop_headers[$header[$i]] = $i;
    }
  }

  while($line = fgetcsv($fp)) {

    if (in_array($line[$biomaterial_header['sample_name']],$biomaterial_names)) {
      print "ERROR: Two BioSamples have the same sample_name. BioSamples must have different sample_names";
      exit(1);
    }
    else {
      $biomaterial_names[] = $line[$biomaterial_header];
    }

    // Gather the fields to insert into the biomaterial table. 
    foreach($biomaterial_header as $name => $index) {
      if ($name == 'sample_name') { 
        $unique_name = $line[$index];
      }
      if ($name == 'description') { 
        $description = $line[$index];
      }
      if ($name == 'biomaterial_provider' and $line[$index] != "") { 
        $biosourceprovider_id = create_biomaterial_contact($line[$index]);
      }
   //   if ($name == 'bioproject_accession') { 
        //$value['primary_dbxref'] = $line[$index];
     // }

    }

    // Create an accession to the bioproject
    $ncbi_bioproject_id = create_ncbi_db('bioproject', 'NCBI BioSample', '');    

    // Create entry in the dbxref table. 
    $dbxref_bioproject= tripal_insert_dbxref(array(
      'db_id' => $ncbi_bioproject_id,
      'accession' => $line[$acc_header['bioproject_accession']],
     // 'version' => $version,
      'description' => '',
      )
    );
    $dbxref_bioproject_id = $dbxref_bioproject->dbxref_id;

    // Create the biomaterial based on the values given.
    $biomaterial_id = create_biomaterial($unique_name, $description, $organism_id, $biosourceprovider_id, '', $description);

    // Insert into the biomaterialprop table.
    foreach($prop_headers as $name => $index) { 
      if (chado_select_record('biomaterialprop', array('biomaterialprop_id'), array('name' => $name))) {
        tripal_insert_cvterm(array(
          'name' => $name,
          'def' => '',
          'cv_name' => 'biomaterial_property',
          'db_name' => 'tripal',
        ));
      }
        	
      $record = array(
        'table' => 'biomaterial',
        'id' => $biomaterial_id,
      );
      $property = array(
        'type_name' => $name,
        'cv_name' => 'biomaterial_property',
        'value' => $line[$index],
      );
      $options = array(
        'update_if_present' => TRUE,
      );
      chado_insert_property($record, $property, $options);
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
    $biosourceprovider_id = create_biomaterial_contact($biomaterial_provider);
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

function create_biomaterial_contact($contact_name) {
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

  // Get the file path. 
  $file_type = $form_state['values']['file_type'];
  if ($file_type == 'xml') {
    $path = $form_state['values']['xml_file']['xml_path'];
    if ($path == '') { form_set_error('xml_file][xml_path', 'Please enter the path to a file.');} //** abs or relative?
    else if (!is_readable($path)) { form_set_error('xml_file][xml_path', 'File is not present or is unreadable.'); }
  }
  else {
    $path = $form_state['values']['flat_file']['flat_path'];
    if ($path == '') { form_set_error('xml_file][xml_path', 'Please enter the path to a file.');} //** abs or relative?
    else if (!is_readable($path)) { form_set_error('flat_file][flat_path', 'File is not present or is unreadable.'); }
  }

  //$xml_path = $form_state['values']['xml_file']['xml_path'];
  //$flat_path = $form_state['values']['flat_file']['flat_path'];
  $flat_delimiter = $form_state['values']['flat_file']['flat_delimiter'];
  $flat_enclosure = $form_state['values']['flat_file']['flat_enclosure'];

  // Make sure the file is present and readable. 
 //*** if (!is_readable($form_sta



}











































