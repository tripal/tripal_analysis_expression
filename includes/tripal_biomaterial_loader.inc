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

  $form['description'] = array( //***
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
      sample_name.</b><br><br>To associate ontology terms (e.g., Plant Ontology, Human Disease Ontology) ') 
      . t('<a href="@url">load an OBO file</a>', array('@url' => '/admin/tripal/loaders/obo_loader'))
      . t(' and then load terms with the ') 
      . t('<a href="@url">bulk loader</a> ', array('@url' => '/node/add/tripal-bulk-loader'))
      . t('(select "Biomaterial Ontology Term Loader" as the template).') 
      . t(' <b>Caution: If a biomaterial with the same name is already in the database,')
      . t(' this loader will update the biomaterial with the parameters set by this form.</b>'));
       
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
      'xml' => t('NCBI BioSample xml file'),
      'flat' => t('Flat file (CSV, TSV)')
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
    '#title' => t('Flat file (CSV, TSV)'),
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
    The only field that is required to create a biomaterial is the name of the biomaterial (column: sample_name). 
    It is recommended that a description (column: description), organism (column: organism), biomaterial provider 
    (column: biomaterial_provider), accession (column: biomaterial_accession), treatment (column: treatment), 
    and tissue (column: tissue) also be provided. A biomaterialprop cvterm type will be created for column 
    titles not already present in the database.<br><br>This loader will create dbxref records for the following 
    column headers if present, biosample_accession, bioproject_accession, and sra_accession. Other accessions should
    be uploaded using a bulk loader template.')
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

  $form['flat_file']['flat_file_type'] = array(
    '#title' => t('Flat file type'),
    '#type' => 'radios',
    //'#required' => TRUE,
    '#states' => array(
      'required' => array(
        ':input[name="file_type"]' => array('value' => 'flat'),
      )
    ),
    '#options' => array(
      'csv' => t('CSV file'),
      'tsv' => t('TSV file')
    ));

/*  $form['flat_file']['flat_delimiter'] = array(
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
  ); */

  $form['flat_file']['flat_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit Job'),
  );

  return $form;
}

function tripal_biomaterial_loader_submit($form, &$form_state) {
//**

  // Get the file path. 
  $file_type = $form_state['values']['file_type'];
  $flat_file_type = $form_state['values']['flat_file']['flat_file_type'];

  if ($file_type == 'xml') {
    $path = $form_state['values']['xml_file']['xml_path'];
  }
  else {
    $path = $form_state['values']['flat_file']['flat_path'];
  }

  $organism_id = $form_state['values']['organism_id'];
 // $flat_delimiter = $form_state['values']['flat_file']['flat_delimiter'];
 // $flat_enclosure = $form_state['values']['flat_file']['flat_enclosure'];

  global $user;

  if ($file_type == 'xml') {

    $job_args = array(
      $path,
      $organism_id
    );

    tripal_add_job("Loading biomaterial data from NCBI XML BioSample file from: $path", 'tripal_analysis_expression',
      'xml_biomaterial_parser', $job_args, $user->uid); 
  }

  if ($file_type == 'flat') {
    $job_args = array(
      $path,
      $organism_id,
      $flat_file_type
    );

    tripal_add_job("Loading biomaterial data from flat file from: $path", 'tripal_analysis_expression',
      'flat_biomaterial_parser', $job_args, $user->uid); 
  } 
}


function flat_biomaterial_parser($flat_path, $organism_id, $flat_file_type) {

  // Check file path to see if file is accessible.nsaction = db_transaction();
  print "\nNOTE: Loading of this expression file is performed using a database transaction. \n" .
        "If the load fails or is terminated prematurely then the entire set of \n" .
        "insertions/updates is rolled back and will not be found in the database\n\n";
  try {  
  if (!is_readable($flat_path)) {
    print "ERROR: File is not readable or not present.";
    exit;
  }

  $num_biosamples = 0;
  $cur_biosample = 0;

  if ($flat_file_type == "tsv") {
    $fgetcsv_values = array($fp,0,"\t");
    $separator = "\t";
    $enclosure = "\0";
  }
  if ($flat_file_type == "csv") {
    $fgetcsv_values = array($fp,0,',','"');
    $separator = ",";
    $enclosure = '"';
  }

  // Count the number of biomaterials in the file.
  $fp = fopen($flat_path, "r");

  while($line = fgetcsv($fp,0,$separator,$enclosure)) {
    foreach($line as $field) { 
      if(preg_match("/(sample_name)/",$field)) {
        break 2;
      }
    }
  }
  while($line = fgets($fp)) {
    $num_biosamples++;
  }
  fclose($fp);

  // Get the file pointer.
  $fp = fopen($flat_path, "r");

  //$headers = fgetcsv($fp);
  while($line = fgetcsv($fp,0,$separator,$enclosure)) {
    foreach($line as $field) { 
      if(preg_match("/(sample_name)/",$field)) {
        break 2;
      }
    }
  }
   
  $headers = $line; 

  // Make sure there are not duplicate biomaterial headers.
  if (count($header_repeats = array_diff_assoc($headers, array_unique($headers))) > 0 ) {
    //if (count($linepart) != count(array_unique($linepart))) {
    print "ERROR: There are multiple instance of the following "
          . "header(s) in the biomaterial file:\n";
    foreach ($header_repeats as $repeat) {
      print "  " . $repeat . "\n";
    }
    print "Please make sure there is only one instance of each "
          . "header in the biomaterial file\n";
    exit;
  }

  $hit = 0;

  $biomaterial_names = [];

  $biomaterial_headers = [];
  $prop_headers = [];
  $acc_headers = [];


  for ($i=0; $i < count($headers); $i++) {
    $header = trim(str_replace("*","",$headers[$i]));
    //drupal_set_message("'" . $header . "'");
    if ($header == 'sample_name') {
      $biomaterial_headers['sample_name'] = $i;
      //drupal_set_message("here ***");
    } //***
    if ($header == 'organism') {
      $biomaterial_headers['organism'] = $i;
    }
    if ($header == 'description') {
      $biomaterial_headers['description'] = $i;
    }
    if ($header == 'biomaterial_provider') {
      $biomaterial_headers['biomaterial_provider'] = $i;
    }
    if ($header == 'biomaterial_accession') {
      $acc_headers['biomaterial_accession'] = $i;
    } 
    if ($header == 'sra_accession') {
      $acc_headers['sra_accession'] = $i;
    }
    if ($header == 'bioproject_accession') {
      $acc_headers['bioproject_accession'] = $i;
    }
    else {
      $prop_headers[$headers[$i]] = $i;
    }
  }

  while($line = fgetcsv($fp,0,$separator,$enclosure)) {

    print "Loading BioSample " . ($cur_biosample+1) . " of " . $num_biosamples . "\n";

    $cur_biosample++;

    $unique_name = '';
    $description = '';
//    $organism_id = '';
    $biosourceprovider_id = '';
    $dbxref_id = '';

    /*if (in_array($line[$biomaterial_headers['sample_name']],$biomaterial_names)) {
      print "ERROR: Two BioSamples have the same sample_name. BioSamples must have different sample_names";
      exit;
    }
    else { */
      $biomaterial_names[] = $line[$biomaterial_headers['sample_name']];
    //}

    // Gather the fields to insert into the biomaterial table. 
    foreach($biomaterial_headers as $name => $index) {
      if ($name == 'sample_name') { 
        $unique_name = $line[$index];
      }
      if ($name == 'description') { 
        $description = $line[$index];
      }
      if ($name == 'biomaterial_provider' and $line[$index] != "") { 
        $biosourceprovider_id = create_biomaterial_contact($line[$index]);
      }
      if ($name == 'biosample_accession') {
        // In case there is no biosample database
        $ncbi_biosample_id = create_ncbi_db('biosample', 'NCBI BioSample', '');    
        $values = array(
          'accession' => $line[$index],
          'db_id' => $ncbi_biosample_id,
        ); 
        $dbxref = tripal_insert_record('dbxref',$values); 
        $dbxref_id = $dbxref->dbxref_id;
        //$value['primary_dbxref'] = $line[$index];
      }

    }

    // Create an accession to the bioproject
  //  $ncbi_bioproject_id = create_ncbi_db('bioproject', 'NCBI BioSample', '');    

    // Create entry in the dbxref table. 
  /*  $dbxref_bioproject= tripal_insert_dbxref(array(
      'db_id' => $ncbi_bioproject_id,
      'accession' => $line[$acc_header['bioproject_accession']],
     // 'version' => $version,
      'description' => '',
      )
    ); 
    $dbxref_bioproject_id = $dbxref_bioproject->dbxref_id;
   */

    // Create the biomaterial based on the values given.
    $biomaterial_id = create_biomaterial($unique_name, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_id, $description);

    // Insert database accessions.
    foreach($acc_headers as $name => $index) {
      // Get the db_id.
      if ($name == 'sra_accession') {
        $db_id = create_ncbi_db('sra', 'NCBI SRA', '');
      }
      else if($name == 'bioproject_accession') { 
        $db_id - create_ncbi_db('bioproject', 'NCBI BioProject', '');     
      }
      // First check to see if there is a dbxref record in the database corresponding to the accession number from the file.
      if (empty(chado_select_record('dbxref', array('dbxref_id'), array('accession' => $line[$index], 'db_id' => $db_id)))) {
        // Insert value into dbxref table.
        $values = array(
          'accession' => $line[$index],
          'db_id' => $db_id,
        );
        $dbxref = tripal_insert_record('dbxref',$values);
        $dbxref_id = $dbxref->dbxref_id;

        // Insert into the biomaterial_dbxref table.
        // Add error checking?
        $values = array(
          'biomaterial_id' => $biomaterial_id,
          'dbxref_id' => $dbxref_id,
        );
        $biomaterial_dbxref = tripal_insert_record('biomaterial_dbxref', $values);
        $biomaterial_dbxref_id = $biomaterial_dbxref->$biomaterial_dbxref_id; 
      }
    }

    // Insert into the biomaterialprop table.
    foreach($prop_headers as $name => $index) { 

      ////
      // Insert into database. If the type is already in the database the function will not update. 
      if(!tripal_insert_cvterm(array(
        'name' => $name,
        'definition' => '',
        'cv_name' => 'biomaterial_property',
        'is_relationship' => 0,
        'db_name' => 'tripal'),
        array('update_existing' => FALSE))) {
        //drupal_set_message("ERROR");
        print "ERROR: Could not insert biomaterial attribute name into the controlled vocabulary table.";
        exit;
      }

      ////

      /*if (chado_select_record('biomaterialprop', array('biomaterialprop_id'), array('name' => $name))) {
        tripal_insert_cvterm(array(
          'name' => $name,
          'def' => '',
          'cv_name' => 'biomaterial_property',
          'db_name' => 'tripal',
        ));
      } */
        	
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

  // Make sure there are not duplicate biomaterials.
  if (count($sample_repeats = array_diff_assoc($biomaterial_names, array_unique($biomaterial_names))) > 0 ) {
    //if (count($linepart) != count(array_unique($linepart))) {
    print "ERROR: There are multiple instance of the following "
          . "sample_name(s) in the biomaterial file:\n";
    foreach ($sample_repeats as $repeat) {
      print "  " . $repeat . "\n";
    }
    print "Please make sure there is only one instance of each "
          . "sample_name in the biomaterial file.\n";
    exit;
  }

  }
  catch (Exception $e) {
    print "\n";
    watchdog_exception('T_expression_load', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
  return;
} 

function xml_biomaterial_parser($xml_path, $organism_id) {

  // Check file path to see if file is accessible.nsaction = db_transaction();
  print "\nNOTE: Loading of this expression file is performed using a database transaction. \n" .
        "If the load fails or is terminated prematurely then the entire set of \n" .
        "insertions/updates is rolled back and will not be found in the database\n\n";
  try {

  if (!is_readable($xml_path)) {
    print "ERROR: File is not readable or not present.";
    exit;
  }

  // Load the contents of the xml file into a string.
  $xml_raw = file_get_contents($xml_path);

  if (!$xml_raw) {
    print "ERROR: Unable to read file.";
    exit;
  }

  // Parse the file.
  $xml = simplexml_load_string($xml_raw);

  $num_biosamples = count($xml->BioSample);

  $biomaterial_names = [];

  // Make sure there are no duplicate biomaterials.
  for ($i=0; $i<$num_biosamples; $i++) {
    $biomaterial_names[] = $xml->BioSample[$i]->Ids->Id[1];
    print $xml->BioSample[$i]->Ids->Id[1];
  }

  // Make sure there are not duplicate biomaterials.
  if (count($sample_repeats = array_diff_assoc($biomaterial_names, array_unique($biomaterial_names))) > 0 ) {
    //if (count($linepart) != count(array_unique($linepart))) {
    print "ERROR: There are multiple instance of the following "
          . "Sample name(s) in the biomaterial file:\n";
    foreach ($sample_repeats as $repeat) {
      print "  " . $repeat . "\n";
    }
    print "Please make sure there is only one instance of each "
          . "Sample name in the biomaterial file.\n";
    exit;
  }

  print "made it this fare";

  exit;

  // Load biomaterials. 
  for ($i=0; $i<$num_biosamples; $i++) {
    print "Loading BioSample " . ($i+1) . " of " . $num_biosamples . "\n";
    add_xml_data($organism_id,$xml->BioSample[$i]);
  }
  }
  catch (Exception $e) {
    print "\n";
    watchdog_exception('T_expression_load', $e);
    $transaction->rollback();
    print "FAILED: Rolling back database changes...\n";
  }
  print "\nDone.\n";
  return;
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

  // Create a contact if contact is not present.

  if ($biomaterial_provider) {
    $biosourceprovider_id = create_biomaterial_contact($biomaterial_provider);
  }
  else {
    $biosourceprovider_id = '';
  }
  // Verify there is an NCBI biosample database.
  $ncbi_biosample_id = create_ncbi_db('biosample', 'NCBI BioSample', '');    

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

  $biomaterial_description = $comment ? (string)$comment : (string)$description;

  $biomaterial_id = create_biomaterial($unique_name, '', $organism_id, $biosourceprovider_id, $dbxref_biosample_id, $biomaterial_description);

  // Add to biomaterialprop table. 
  add_xml_biomaterial_properties($biosample->Attributes->Attribute, $biomaterial_id);

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

    // Get the cvterm name 
    $attr_name = (string)$attr->attributes()->attribute_name;

    if ($attr_name == "biomaterial_provider") {
      continue;
    }

    // Insert into database. If the type is already in the database the function will not update. 
    if(!tripal_insert_cvterm(array(
      'name' => (string)$attr_name, 
      'definition' => '',
      'cv_name' => 'biomaterial_property', 
      'is_relationship' => 0,
      'db_name' => 'tripal'), 
      array('update_existing' => FALSE))) {
      //drupal_set_message("ERROR");
      print "ERROR: Could not insert biomaterial attribute name into the controlled vocabulary table.";
      exit;
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
      print "ERROR: Unable to write to contact table\n";
      exit;
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
      print "ERROR: Unable to write to db table\n";
      exit;
    }
    $db_id = $db['db_id'];
  }

  if($db_id) {
    $db = chado_update_record('db',
      array('name' => 'db'),
      $values,
      array('return_record' => TRUE));
    if($db==FALSE) {
      print "ERROR: Unable to update db_table\n";
      exit;
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
  //$flat_delimiter = $form_state['values']['flat_file']['flat_delimiter'];
  //$flat_enclosure = $form_state['values']['flat_file']['flat_enclosure'];

  // Make sure the file is present and readable. 
 //*** if (!is_readable($form_sta



}











































