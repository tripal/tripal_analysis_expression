<?php

class tripal_biomaterial_loader_v3 extends TripalImporter{

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Chado Biological Sample (Biomaterial) Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'chado_biosample_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Import biological samples (Biomaterials) from NCBI BioSamples';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['xml', 'tsv', 'csv'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Please upload an NCBI BioSample file.  This can be in XML with an .xml extension, or "flat file" format with a tsv or csv extension.';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'New NCBI BioSample file';

  /**
   * If the loader should require an analysis record.  To maintain provenance
   * we should always indicate where the data we are uploading comes from.
   * Tripal attempts to use this by associating upload files
   * with an analysis record.  The analysis record provides the details for
   * how the file was created or obtained. Set this to FALSE if the loader
   * should not require an analysis when loading. if $use_analysis is set to
   * true then the form values will have an 'analysis_id' key in the $form_state
   * array on submitted forms.
   */
  public static $use_analysis = TRUE;

  /**
   * If the $use_analysis value is set above then this value indicates if the
   * analysis should be required.
   */
  public static $require_analysis = TRUE;

  /**
   * Text that should appear on the button at the bottom of the importer
   * form.
   */
  public static $button_text = 'Import NCBI BioSample File';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  public static $file_required = TRUE;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    // get a list of organisms for the user to choose
    $sql = "SELECT * FROM {organism} ORDER BY genus, species";
    $results = chado_query($sql);
    $organisms = [];
    foreach ($results as $org) {
      $organisms[$org->organism_id] = $org->common_name;
    }

    $form['description'] = [ //***
      '#markup' => t('Description for the biomaterial / biosample loader'),
    ];

    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#required' => TRUE,
      '#description' => t('The organism from which the biosamples were collected.'),
      '#options' => $organisms,
    ];

    $form['file_type'] = [
      '#title' => t('File type'),
      '#type' => 'radios',
      '#required' => TRUE,
      '#options' => [
        'xml' => t('NCBI BioSample xml file'),
        'flat' => t('Flat file (CSV, TSV)'),
      ],
    ];

    // Flat file will require a regexp

    $form['flat_file'] = [
      '#type' => 'fieldset',
      '#title' => t('NCBI biosample flat file'),
      '#states' => [
        'visible' => [
          ':input[name="file_type"]' => ['value' => 'flat'],
        ],
      ],
    ];

    $form['flat_file']['regex_description'] = [
      '#markup' => t('The flat file loader expects a file where the first line is the title of each column. 
    The only field that is required to create a biomaterial is the name of the biomaterial (column: sample_name). 
    It is recommended that a description (column: description), organism (column: organism), biomaterial provider 
    (column: biomaterial_provider), accession (column: biomaterial_accession), treatment (column: treatment), 
    and tissue (column: tissue) also be provided. A biomaterialprop cvterm type will be created for column 
    titles not already present in the database.<br><br>This loader will create dbxref records for the following 
    column headers if present, biosample_accession, bioproject_accession, and sra_accession. Other accessions should
    be uploaded using a bulk loader template.'),
    ];

    $form['flat_file']['flat_file_type'] = [
      '#title' => t('Flat file type'),
      '#type' => 'radios',
      //'#required' => TRUE,
      '#states' => [
        'required' => [
          ':input[name="file_type"]' => ['value' => 'flat'],
        ],
      ],
      '#options' => [
        'csv' => t('CSV file'),
        'tsv' => t('TSV file'),
      ],
    ];

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    dpm($form_state);

    // Make sure an organism is selected.
    $organism_id = $form_state['values']['organism_id'];
    $file_type = $form_state['values']['file_type'];
    if (!$organism_id) {
      form_set_error('organism_id', 'Please select the organism from which the biomaterials were collected.');
    }
    // Make sure the flat file type is selected when loading a flat file.
    $flat_file_type = $form_state['values']['flat_file_type'];


    if ($file_type == 'flat' and !$flat_file_type) {
      form_set_error('flat_file_type', 'Please select a flat file format.');
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['file']['file_path'];
    $organism_id = $arguments['organism_id'];
    $file_type = $arguments['file_type'];
    $analysis_id = $arguments['analysis_id'];
    $flat_file_type = $arguments['flat_file_type'];

    if ($file_type == "xml") {
      $this->load_biosample_xml($file_path, $organism_id, $analysis_id);
    }
    else {
      $this->load_biosample_flat($file_path, $organism_id, $analysis_id, $flat_file_type);
    }

  }

  /**
   * @param $file_path
   * @param $organism_id
   * Loads biosample from XML file.
   */
  private function load_biosample_xml($file_path, $organism_id, $analysis_id) {

    // Check file path to see if file is accessible.nsaction = db_transaction();
    try {
      if (!is_readable($file_path)) {
        print "ERROR: File is not readable or not present.";
        exit;
      }

      // check if the file given is empty
      $fileSize = filesize($file_path);
      if ($fileSize == 0) {
        print("ERROR: File at $file_path is empty. Try again with a new file.\n");
        exit;
      }

      // Load the contents of the xml file into a string.
      $xml_raw = file_get_contents($file_path);

      if (!$xml_raw) {
        print "ERROR: Unable to read file.";
        exit;
      }

      // Parse the file.
      $xml = simplexml_load_string($xml_raw);

      $num_biosamples = count($xml->BioSample);

      if ($num_biosamples == 0) {
        print("ERROR: wrong format at $file_path. XML file must include'BioSample' tag\n");
        exit;
      }

      $biomaterial_names = [];

      // Make sure there are no duplicate biomaterials.
      for ($i = 0; $i < $num_biosamples; $i++) {
        $biomaterial_names[] = $xml->BioSample[$i]->Ids->Id[1];
      }

      // Make sure there are not duplicate biomaterials.
      if (count($sample_repeats = array_diff_assoc($biomaterial_names, array_unique($biomaterial_names))) > 0) {
        //if (count($linepart) != count(array_unique($linepart))) {
        print "ERROR: There are multiple instance of the following " . "Sample name(s) in the biomaterial file:\n";
        foreach ($sample_repeats as $repeat) {
          print "  " . $repeat . "\n";
        }
        print "Please make sure there is only one instance of each " . "Sample name in the biomaterial file.\n";
        exit;
      }

      // Load biomaterials.
      for ($i = 0; $i < $num_biosamples; $i++) {
        print "Loading BioSample " . ($i + 1) . " of " . $num_biosamples . "\n";
        add_xml_data($organism_id, $xml->BioSample[$i], $analysis_id);
      }
    } catch (Exception $e) {
      print "\n";
      watchdog_exception('T_expression_load', $e);
      //$transaction->rollback();
      print "FAILED: Rolling back database changes...\n";
    }
    print "\nDone.\n";
    return;
  }

  /**
   * @param $organism_id
   * @param $biosample
   */
  function add_xml_data($organism_id, $biosample, $analysis_id) {

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
      $biosourceprovider_id = $this->create_biomaterial_contact($biomaterial_provider);
    }
    else {
      $biosourceprovider_id = '';
    }
    // Verify there is an NCBI biosample database.
    $ncbi_biosample_id = $this->create_ncbi_db('biosample', 'NCBI BioSample', '');

    // Verify there is an NCBI sra database.
    $ncbi_sra_id = $this->create_ncbi_db('sra', 'NCBI SRA', '');

    // Create entry in the dbxref table.
    $dbxref_biosample = tripal_insert_dbxref([
      'db_id' => $ncbi_biosample_id,
      'accession' => $sample_accession,
      // 'version' => $version,
      'description' => '',
    ]);
    $dbxref_biosample_id = $dbxref_biosample->dbxref_id;

    // If sra_accession is present, create entry in the dbxref table.
    if ($sra_accession) {
      $dbxref_sra = tripal_insert_dbxref([
        'db_id' => $ncbi_sra_id,
        'accession' => $sra_accession,
        // 'version' => $version,
        'description' => '',
      ]);
      $dbxref_sra_id = $dbxref_sra->dbxref_id;
    }

    $biomaterial_description = $comment ? (string) $comment : (string) $description;

    $biomaterial_id = $this->create_biomaterial($unique_name, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_biosample_id, $biomaterial_description);

    // Add to biomaterialprop table.
    add_xml_biomaterial_properties($biosample->Attributes->Attribute, $biomaterial_id);

    // Add to biomaterial_dbxref table.
    if ($sra_accession) {
      tripal_associate_dbxref('biomaterial', $biomaterial_id, [
        'accession' => $sra_accession,
        'db_name' => 'NCBI SRA',
      ]);
    }
  }

  /**
   * @param $attributes
   * @param $biomaterial_id
   */
  function add_xml_biomaterial_properties($attributes, $biomaterial_id) {

    $record = [
      'table' => 'biomaterial',
      'id' => $biomaterial_id,
    ];

    $options = [
      'update_if_present' => TRUE,
    ];

    // Iterate through each property
    foreach ($attributes as $attr) {

      // Get the cvterm name
      $attr_name = (string) $attr->attributes()->attribute_name;

      if ($attr_name == "biomaterial_provider") {
        continue;
      }

      // Insert into database. If the type is already in the database the function will not update.
      if (!tripal_insert_cvterm([
        'name' => (string) $attr_name,
        'definition' => '',
        'cv_name' => 'biomaterial_property',
        'is_relationship' => 0,
        'db_name' => 'tripal',
      ], ['update_existing' => FALSE])) {
        //drupal_set_message("ERROR");
        print "ERROR: Could not insert biomaterial attribute name into the controlled vocabulary table.";
        exit;
      }

      // Insert the property into the biomaterialprop table.
      $property = [
        'type_name' => $attr_name,
        'cv_name' => 'biomaterial_property',
        'value' => $attr,
      ];
      chado_insert_property($record, $property, $options);
    }
  }

  /**
   * @param $file_path
   * @param $organism_id
   * @param $analysis_id
   * @param $file_type
   * @param $flat_file_type
   */


  private function load_biosample_flat(
    $file_path,
    $organism_id,
    $analysis_id,
    $flat_file_type
  ) {


    try {
      if (!is_readable($file_path)) {
        print "ERROR: File is not readable or not present.";
        exit;
      }

      $num_biosamples = 0;
      $cur_biosample = 0;

      if ($flat_file_type == "tsv") {
        $fgetcsv_values = [$fp, 0, "\t"];
        $separator = "\t";
        $enclosure = "\0";
      }

      if ($flat_file_type == "csv") {
        $fgetcsv_values = [$fp, 0, ',', '"'];
        $separator = ",";
        $enclosure = '"';
      }

      // check if the file given is empty
      $fileSize = filesize($file_path);
      if ($fileSize == 0) {
        print("ERROR: File at $file_path is empty. Try again with a new file.\n");
        exit;
      }

      // Count the number of biomaterials in the file.
      $fp = fopen($file_path, "r");
      while ($line = fgetcsv($fp, 0, $separator, $enclosure)) {
        foreach ($line as $field) {
          if (preg_match("/(sample_name)/", $field)) {
            break 2;
          }
        }
      }

      while ($line = fgets($fp)) {
        $num_biosamples++;
      }
      fclose($fp);

      //print error message and exit if there's no biosample, or that there's no "sample_name" column in flat file
      if ($num_biosamples == 0) {
        print("ERROR: Wrong file format at $file_path. File must contain a column named 'sample_name'.\n");
        print("Please try again with a file that contains at least one column named 'sample_name' in its header line, followed by lines of biosample data.\n");
        exit;
      }

      // Get the file pointer.
      $fp = fopen($file_path, "r");

      //$headers = fgetcsv($fp);
      while ($line = fgetcsv($fp, 0, $separator, $enclosure)) {
        foreach ($line as $field) {
          if (preg_match("/(sample_name)/", $field)) {
            break 2;
          }
        }
      }
      $headers = $line;

      // Make sure there are not duplicate biomaterial headers.
      if (count($header_repeats = array_diff_assoc($headers, array_unique($headers))) > 0) {
        //if (count($linepart) != count(array_unique($linepart))) {
        print "ERROR: There are multiple instance of the following " . "header(s) in the biomaterial file:\n";
        foreach ($header_repeats as $repeat) {
          print "  " . $repeat . "\n";
        }
        print "Please make sure there is only one instance of each " . "header in the biomaterial file\n";
        exit;
      }

      $hit = 0;

      $biomaterial_names = [];

      $biomaterial_headers = [];
      $prop_headers = [];
      $acc_headers = [];

      for ($i = 0; $i < count($headers); $i++) {
        $header = trim(str_replace("*", "", $headers[$i]));
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

      while ($line = fgetcsv($fp, 0, $separator, $enclosure)) {

        print "Loading BioSample " . ($cur_biosample + 1) . " of " . $num_biosamples . "\n";

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
        foreach ($biomaterial_headers as $name => $index) {
          if ($name == 'sample_name') {
            $unique_name = $line[$index];
          }
          if ($name == 'description') {
            $description = $line[$index];
          }
          if ($name == 'biomaterial_provider' and $line[$index] != "") {
            $biosourceprovider_id = $this->create_biomaterial_contact($line[$index]);
          }
          if ($name == 'biosample_accession') {
            // In case there is no biosample database
            $ncbi_biosample_id = $this->create_ncbi_db('biosample', 'NCBI BioSample', '');
            $values = [
              'accession' => $line[$index],
              'db_id' => $ncbi_biosample_id,
            ];
            $dbxref = chado_insert_record('dbxref', $values);
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
        $biomaterial_id = $this->create_biomaterial($unique_name, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_id, $description);

        // Insert database accessions.
        foreach ($acc_headers as $name => $index) {
          // Get the db_id.
          if ($name == 'sra_accession') {
            $db_id = $this->create_ncbi_db('sra', 'NCBI SRA', '');
          }
          else {
            if ($name == 'bioproject_accession') {
              $db_id = $this->create_ncbi_db('bioproject', 'NCBI BioProject', '');
            }
          }
          // First check to see if there is a dbxref record in the database corresponding to the accession number from the file.
          $record = chado_select_record('dbxref', ['dbxref_id'], [
            'accession' => $line[$index],
            'db_id' => $db_id,
          ]);
          if (empty($record)) {
            // Insert value into dbxref table.
            $values = [
              'accession' => $line[$index],
              'db_id' => $db_id,
            ];
            $dbxref = chado_insert_record('dbxref', $values);
            $dbxref_id = $dbxref['dbxref_id'];

            // Insert into the biomaterial_dbxref table.
            // Add error checking?
            $values = [
              'biomaterial_id' => $biomaterial_id,
              'dbxref_id' => $dbxref_id,
            ];
            $biomaterial_dbxref = chado_insert_record('biomaterial_dbxref', $values);
            $biomaterial_dbxref_id = $biomaterial_dbxref['biomaterial_dbxref_id'];
          }
        }

        // Insert into the biomaterialprop table.
        foreach ($prop_headers as $name => $index) {

          ////
          // Insert into database. If the type is already in the database the function will not update.
          if (!tripal_insert_cvterm([
            'name' => $name,
            'definition' => '',
            'cv_name' => 'biomaterial_property',
            'is_relationship' => 0,
            'db_name' => 'tripal',
          ], ['update_existing' => FALSE])) {
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

          $record = [
            'table' => 'biomaterial',
            'id' => $biomaterial_id,
          ];
          $property = [
            'type_name' => $name,
            'cv_name' => 'biomaterial_property',
            'value' => $line[$index],
          ];
          $options = [
            'update_if_present' => TRUE,
          ];
          chado_insert_property($record, $property, $options);
        }
      }

      // Make sure there are not duplicate biomaterials.
      if (count($sample_repeats = array_diff_assoc($biomaterial_names, array_unique($biomaterial_names))) > 0) {
        //if (count($linepart) != count(array_unique($linepart))) {
        print "ERROR: There are multiple instance of the following " . "sample_name(s) in the biomaterial file:\n";
        foreach ($sample_repeats as $repeat) {
          print "  " . $repeat . "\n";
        }
        print "Please make sure there is only one instance of each " . "sample_name in the biomaterial file.\n";
        exit;
      }
    } catch (Exception $e) {
      print "\n";
      watchdog_exception('T_expression_load', $e);
      //$transaction->rollback();
      print "FAILED: Rolling back database changes...\n";
    }
    print "\nDone.\n";
    return;
  }

  private function create_biomaterial_contact($contact_name) {
    $contact_id = chado_query('select contact_id from {contact} where name = :name limit 1', [':name' => $contact_name])->fetchField();
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

  private function create_ncbi_db($url_name, $db_name, $db_description) {

    $url = "http://www.ncbi.nlm.nih.gov/";
    $urlprefix = "http://www.ncbi.nlm.nih.gov/" . $url_name . "/";

    $values = [
      'name' => $db_name,
      'description' => $db_description,
      'urlprefix' => $urlprefix,
      'url' => $url,
    ];

    $db_id = chado_query('select db_id from {db} where name = :name limit 1', [':name' => $db_name])->fetchField();

    if (!$db_id) {
      $db = chado_insert_record('db', $values);
      if ($db == FALSE) {
        print "ERROR: Unable to write to db table\n";
        exit;
      }
      $db_id = $db['db_id'];
    }

    if ($db_id) {
      $db = chado_update_record('db', ['name' => 'db'], $values, ['return_record' => TRUE]);
      if ($db == FALSE) {
        print "ERROR: Unable to update db_table\n";
        exit;
      }
    }

    return $db_id;
  }

  /**
   * @param $biomaterial
   * @param $analysis_id
   * @param $organism_id
   * @param $biosourceprovider_id
   * @param $dbxref_id
   * @param $biomaterial_description
   *
   * @return mixed
   */
  private function create_biomaterial($biomaterial, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_id, $biomaterial_description) {
    $biomaterial_result = chado_query('select biomaterial_id, taxon_id, description from {biomaterial} where name = :name limit 1', [':name' => $biomaterial])->fetchObject();

    // if sample_name already exists in chado db, print warning message
    if ($biomaterial_result->taxon_id) {
      print("WARNING: Biomaterial with sample_name of '$biomaterial' alreay exists in the database. Overwriting database entries for $biomaterial...\n");
    }

    if ($biomaterial_result->taxon_id and $biomaterial_result->taxon_id != $organism_id) {
      $sql = "SELECT common_name, genus, species FROM {organism} WHERE organism_id = :organism_id";
      $analysis_org = chado_query($sql, [":organism_id" => $organism_id])->fetchObject();
      $biomaterial_org = chado_query($sql, [":organism_id" => $biomaterial_result->taxon_id])->fetchObject();
      print "ERROR: Mismatch between the organism selected in the loader " . "and the organism of a biomaterial in the Chado database.\n" . "  Organism selected by loader: " . $analysis_org->genus . " " . $analysis_org->species . " (" . $analysis_org->common_name . ")\n" . "  Biomaterial organism: " . $biomaterial_org->genus . " " . $biomaterial_org->species . " (" . $biomaterial_org->common_name . ")\n" . "  On biomaterial: " . $biomaterial . "\n" . "Please ensure that the organism selected by the loader " . "and the organism for each biomaterial in the file to be loaded " . "are the same. If these organisms should not be the same, " . "delete the offending biomaterial in Chado or change the name of " . "the biomaterial to another unique name.\n";
      exit;
    }

    $biomaterial_id = $biomaterial_result->biomaterial_id;

    if ($analysis_id) {
      $analysis_name = chado_select_record('analysis', ['name'], ['analysis_id' => $analysis_id]);
    }

    if ($biomaterial_description == '' and $biomaterial_result->description != '') {
      $biomaterial_description = '';
    }

    elseif ($biomaterial_description == '' && $analysis_id) {
      $biomaterial_description = 'This biomaterial: ' . $biomaterial . ', was created for the analysis: ' . $analysis_name[0]->name;
    }

    $values = [
      'name' => $biomaterial,
      //'description' => $biomaterial_description,
      //'biosourceprovider_id' => $biosourceprovider_id,
      'taxon_id' => $organism_id,
      //  'dbxref_id' => $dbxref_id,
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

    if ($biomaterial_id) {
      print("Updating record for biomaterial id = $biomaterial_id\n");
      $biomaterial = chado_update_record('biomaterial', ['name' => $biomaterial], $values, ['return_record' => TRUE]);
      if ($biomaterial == FALSE) {
        print "ERROR: Unable to update biomaterial_table\n";
        exit;
      }
    }
    $biomaterial_id = $biomaterial['biomaterial_id'];

    return $biomaterial_id;
  }

}
