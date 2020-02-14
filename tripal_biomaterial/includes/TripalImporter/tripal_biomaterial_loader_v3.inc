<?php

/**
 * @file
 */

/**
 *
 */
class tripal_biomaterial_loader_v3 extends TripalImporter {

  public static $name = 'Chado Biological Sample (Biomaterial) Loader';

  public static $machine_name = 'chado_biosample_loader';

  public static $description = 'Import biological samples (Biomaterials) from NCBI BioSamples';

  public static $file_types = ['xml', 'tsv', 'csv'];

  public static $upload_description = "Please upload an NCBI BioSample file.  This can be in XML with an .xml extension, or flat file format with a .tsv or .csv extension.<br><br> If loading a CSV/TSV flat file, the first line must be the column name.  The only field that is required to create a biosample is the name (column: sample_name). It is recommended that a description (column: description), biomaterial provider (column: biomaterial_provider), accession (column: biomaterial_accession), treatment (column: treatment), and tissue (column: tissue) also be provided. A biomaterialprop cvterm type will be created for column
    titles not associated with a cvterm below.  <br>This loader will create dbxref records for the following
    column headers if present: biosample_accession, bioproject_accession, and sra_accession. Other accessions should
    be uploaded using a bulk loader template. ";

  public static $upload_title = 'Import new Biosamples/Biomaterials';

  public static $use_analysis = TRUE;

  public static $require_analysis = FALSE;

  public static $button_text = 'Import Biosamples';

  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    /**
     * Allow the user to provide a remote URL for the file.
     */
    'file_remote' => FALSE,
  ];

  public static $file_required = TRUE;


  /**
   * Organism associated with biomaterial.
   *
   * @var int
   */
  private static $organism_id;

  /**
   * Analysis associated with the biomaterial.
   *
   * @var int
   */
  private static $analysis_id;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    $organisms = chado_get_organism_select_options(FALSE);

    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#required' => TRUE,
      '#default_value' => 0,
      '#description' => t('The organism from which the biosamples were collected.  Each upload must consist of samples from the same organism.'),
      '#options' => $organisms,

    ];

    $form['biomaterial_cvterm_validator'] = [
      '#type' => 'button',
      '#value' => t('Check Biosamples'),
      '#ajax' => [
        'callback' => 'cvterm_validator_callback',
        'wrapper' => 'cvterm_configuration_div',
      ],
      '#submit' => ['cvterm_validator_callback_submit'],
    ];

    $fields = [];
    $property_values = [];

    $description = t('After submitting uploading your file(s) and clicking the Check Biosamples button, you can configure individual cvterms for your biomaterial properties here.');

    if ((isset($form_state['values']['file_local'])) || isset($form_state['values']['file_upload']) || isset($form_state['values']['file_upload_existing'])) {
      $file_path = $this->findFile($form_state['values']);

      $extension = explode('.', $file_path);
      $extension = $extension[count($extension) - 1];
      $file_type = $extension;
      $organism_id = $form_state['values']['organism_id'];
      $analysis_id = $form_state['values']['analysis_id'];
      $link = variable_get('website_base_url') . "admin/tripal/loaders/chado_cv/cvterm/add.";
      $description = t('This section will allow you to check the CVterms associated with your biomaterial.  Ideally, each property should get the term for type and value from a Controlled Vocabulary (CV).  Alternatively you can create ad hoc terms in the biomaterialprop CV.  If the CVterm does not exist in a suitable CV, you can insert terms <a href = "' . $link . '"> using the Tripal CVterm loader.</a>');

      $terms_to_check = NULL;
      if ($file_type == 'xml') {
        $terms_to_check = $this->parse_xml_biomaterial_file($file_path, $organism_id, $analysis_id);
      }
      else {
        $terms_to_check = test_biosample_cvterms_flat($file_path);
      }
      $fields = $terms_to_check["attributes"];
      $property_values = $terms_to_check["values"];
    }

    $form['cvterms'] = [
      '#type' => 'fieldset',
      '#prefix' => '<div id="cvterm_configuration_div">',
      '#suffix' => '</div>',
    ];

    $form['cvterms']['cvterm_configuration'] = [
      '#type' => 'fieldset',
      '#title' => t('CVterm Field Configuration'),
      '#description' => $description,

      '#tree' => TRUE,
    ];

    $description = t('This section will allow you to check the CVterms associated with your biomaterial property <b>values</b>.  FOr example, if your property was "color", the property value might be "red".</a>');

    $form['cvterms']['cvalue_configuration'] = [
      '#type' => 'fieldset',
      '#title' => t('CVterm Value Configuration'),
      '#description' => $description,
      '#tree' => TRUE,
    ];

    // Build the property cvterm and cvalue portion of the form.
    foreach ($fields as $field) {
      if (is_string($field)){
        $field = [$field];
      }

      $form = $this->build_term_form_item($form, $field, "cvterm_configuration");
    }

    foreach ($property_values as $property_value) {
      if (is_string($property_value)){
        $property_value = [$property_value];
      }
      $form = $this->build_term_form_item($form, $property_value, "cvalue_configuration");
    }

    return $form;
  }

  /**
   * Builds the property to term mapping form area.  Reused for cvterms and
   * cvalues.
   *
   * @param array $form
   *   The drupal form array.
   * @param string $property_value
   *   The name of property that will be checked for cvterms.
   * @param string $cvterm_or_cvalue
   *   The piece of the form array to build in.
   *    Should be 'cvalue_configuration' or 'cvterm_configuration'.
   *
   * @return mixed
   *
   * @throws \Exception
   */
  private function build_term_form_item(array $form, array $property_value, string $cvterm_or_cvalue) {
    $cv_array = [];

    // Get terms.
    $sql = "SELECT CV.name AS cv_name, DB.name AS db_name, DBX.accession AS dbx_accession, CVT.name AS cvt_name, CVT.definition AS cvt_def, CVT.cvterm_id AS cvt_id  FROM {cvterm} AS CVT
              INNER JOIN {CV} AS CV ON CVT.cv_id = CV.cv_id
              INNER JOIN {dbxref} as DBX ON CVT.dbxref_id = DBX.dbxref_id
              INNER JOIN {db} as DB ON DBX.db_id = DB.db_id
              WHERE CVT.name  IN (:field)";

    $prop_result = chado_query($sql, [":field" => $property_value])->fetchAll();

    foreach ($prop_result as $field_term) {
      $cv_name = $field_term->cv_name;
      $db_name = $field_term->db_name;
      $accession = $field_term->dbx_accession;
      $cvt_name = $field_term->cvt_name;
      $cvt_def = $field_term->cvt_def;

      $cv_array[$cv_name]["cv"] = $cv_name;
      $cv_array[$cv_name]["db"] = $db_name;
      $cv_array[$cv_name]["accession"] = $accession;
      $cv_array[$cv_name]["cvt_name"] = $cvt_name;
      $cv_array[$cv_name]["cvt_def"] = $cvt_def;
      $cv_array[$cv_name]["cvt_id"] = $field_term->cvt_id;

    }

    $field_label = reset($property_value);

    $form['cvterms'][$cvterm_or_cvalue][$field_label] = [
      '#type' => 'fieldset',
      '#title' => t($field_label),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ];
    $header = [
      "CVterm name",
      "CV name",
      "DB name",
      "Accession",
      "CVterm definition",
    ];

    $rows = [];
    foreach ($cv_array as $cv_name => $field_term) {
      $db_name = $field_term["db"];
      $accession = $field_term["accession"];
      $cvt_name = $cv_array[$cv_name]["cvt_name"];
      $cvt_def = $cv_array[$cv_name]["cvt_def"];
      $cvt_id = $cv_array[$cv_name]["cvt_id"];

      $row = [$cvt_name, $cv_name, $db_name, $accession, $cvt_def];
      $rows[$cv_name] = $row;

      $info = [
        'cvt_name' => $cvt_name,
        'cv_name' => $cv_name,
        'db_name' => $db_name,
        'cvt_accession' => $accession,
        'cvt_id' => $cvt_id,
      ];

      // Stick the info in the form so we can match it up later.
      $form['cvterms'][$cvterm_or_cvalue][$field_label][$cv_name] = [
        '#type' => 'value',
        '#value' => $info,
      ];

    }

    // If no rows then the term isn't in DB.
    if (!$rows) {
      $markup = "<p>There is no cvterm in the Chado database matching the property <b>$field_label</b>.  You can insert a term and try again, or submit and use a generic term.</p>";
      $form['cvterms'][$cvterm_or_cvalue][$field_label]['choose'] = [
        '#markup' => $markup,
      ];
    }
    else {

      $form['cvterms'][$cvterm_or_cvalue][$field_label]['choose'] = [
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $rows,
        '#multiple' => FALSE,
      ];
    }

    return $form;
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['files']['file_path'];
    if (array_key_exists('file_path', $this->arguments['files'][0])) {
      $file_path = $this->arguments['files'][0]['file_path'];
    }
    $this->organism_id = $arguments['organism_id'];
    $this->analysis_id = $arguments['analysis_id'];
    $field_info = $arguments['cvterm_configuration'];
    $cvalue_info = $arguments['cvalue_configuration'];

    // Generate list of cvterm properties to associate with each field
    // if one wasnt selected, key wont exist and we'll insert into biomaterialprop.
    $insert_fields = [];
    if ($field_info) {
      foreach ($field_info as $property_label => $property) {
        $selected_cv = $property['choose'];
        $selected_cv_info = $property[$selected_cv];
        if ($selected_cv) {
          $insert_fields[$property_label] = $selected_cv_info;
        }
      }
    }
    else {
      $this->logMessage("Properties not configured.  Using generic cvterms for all properties.",
        [], TRIPAL_WARNING);
    }

    $insert_cvalues = [];

    if ($cvalue_info) {
      foreach ($cvalue_info as $cvalue_label => $property) {

        $selected_cv = $property['choose'];
        if ($selected_cv) {
          $selected_cv_info = $property[$selected_cv];
          $insert_cvalues[$cvalue_label] = $selected_cv_info['cvt_id'];
        }
      }
    }

    if (array_key_exists('file_remote', $this->arguments['files'][0])) {
      $extension = explode('.', $this->arguments['files'][0]['file_remote']);
      $extension = $extension[count($extension) - 1];
    }
    else {
      $extension = explode('.', $file_path);
      $extension = $extension[count($extension) - 1];
    }

    if ($extension == "xml") {
      $this->load_biosample_xml($file_path, $insert_fields, $insert_cvalues);
    }
    else {
      $this->load_biosample_flat($file_path, $insert_fields, $insert_cvalues);
    }
  }

  /**
   * @param $file_path
   * @param  $insert_fields
   * @param $insert_cvalues
   *
   *   Loads biosample from XML file.
   */
  protected function load_biosample_xml(
    $file_path,
    $insert_fields,
    $insert_cvalues
  ) {

    $organism_id = $this->organism_id;
    $analysis_id = $this->analysis_id;

    $xml = $this->read_xml_file($file_path);

    $num_biosamples = count($xml->BioSample);

    // Load biomaterials.
    for ($i = 0; $i < $num_biosamples; $i++) {
      $this->logMessage("Loading BioSample !num of !total",
        ['!num' => ($i + 1), '!total' => $num_biosamples]);
      $this->add_xml_data($xml->BioSample[$i], $analysis_id, $insert_fields, $insert_cvalues);
    }
  }

  /**
   * @param $biosample
   * @param $analysis_id
   * @param $insert_fields
   * @param $insert_cvalues
   *   - the cvalues to associate with the
   *   biomaterialprop.  Use this in addition to the property text value.
   */
  protected function add_xml_data(
    $biosample,
    $analysis_id,
    $insert_fields,
    $insert_cvalues
  ) {
    $organism_id = $this->organism_id;

    // Extract data from the xml string.
    $sample_accession = $biosample->Ids->Id[0];
    $unique_name = $biosample->Ids->Id[1];
    $sra_accession = $biosample->Ids->Id[2];
    $description = $biosample->Description->Title;

    // Right now we use the organism_id on the form instead of the organism value from the XML here.  If the user was uploading an xml file with mixed organisms, this would be problematic.
    // $organism = $biosample->Description->Organism->OrganismName;.
    $comment = $biosample->Description->Comment->Paragraph;
    $ncbi_owner = $biosample->Owner->Name;

    $biomaterial_provider = $biosample->Owner->Contacts->Contact->Name;

    // Create a contact if contact is not present.
    if ($biomaterial_provider) {
      if ($biomaterial_provider->Last && $biomaterial_provider->First) {
        $contact_name = $biomaterial_provider->Last . ", " . $biomaterial_provider->First;
      }
      elseif ($biomaterial_provider->Last) {
        $contact_name = $biomaterial_provider->Last;

      }
      else {
        $contact_name = $biomaterial_provider;
      }
      $biosourceprovider_id = tripal_biomaterial_create_biomaterial_contact($contact_name);
    }
    else {
      $biosourceprovider_id = '';
    }
    // Verify there is an NCBI biosample database.
    $ncbi_biosample_id = tripal_biomaterial_create_ncbi_db('biosample', 'NCBI BioSample', '');

    // Verify there is an NCBI sra database.
    $ncbi_sra_id = tripal_biomaterial_create_ncbi_db('sra', 'NCBI SRA', '');

    // Create entry in the dbxref table.
    $dbxref_biosample = tripal_insert_dbxref([
      'db_id' => $ncbi_biosample_id,
      'accession' => $sample_accession,
      // 'version' => $version,.
      'description' => '',
    ]);
    $dbxref_biosample_id = $dbxref_biosample->dbxref_id;

    $dbxref_id = NULL;
    // note:  this dbxref (ncbi biosample) previously was insert into the biomaterial table.  This is incorrect: t his column is only for INTERNAL accessions.
    // If sra_accession is present, create entry in the dbxref table.
    if ($sra_accession) {
      $dbxref_sra = tripal_insert_dbxref([
        'db_id' => $ncbi_sra_id,
        'accession' => $sra_accession,
        // 'version' => $version,.
        'description' => '',
      ]);
    }

    $biomaterial_description = $comment ? (string) $comment : (string) $description;

    $biomaterial_id = tripal_biomaterial_create_biomaterial($unique_name, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_id, $biomaterial_description);

    // Add to biomaterialprop table.
    $this->add_xml_biomaterial_properties($biosample->Attributes->Attribute, $biomaterial_id, $insert_fields, $insert_cvalues);

    // Add to biomaterial_dbxref table.
    if ($dbxref_sra) {
      tripal_associate_dbxref('biomaterial', $biomaterial_id, [
        'accession' => $sra_accession,
        'db_name' => 'NCBI SRA',
      ]);
    }
    // Also add the NCBI BioSample DBXREF previously inserted into the biomaterial table.
    if ($dbxref_biosample) {
      tripal_associate_dbxref('biomaterial', $biomaterial_id, [
        'accession' => $sample_accession,
        'db_name' => 'NCBI BioSample',
      ]);
    }
  }

  /**
   * @param $ids
   *   the IDs subset of the XML.  For example,
   *   <Ids>
   *   <Id db="BioSample" is_primary="1">professional</Id>
   *   </Ids>
   *
   * @return String, the biomaterial ID string value.
   */
  private function find_primary_ID($ids) {

    if (count($ids) === 1) {
      return (string) $ids[0]->Id;
    }
    foreach ($ids as $id) {
      $attributes = $id->attributes();
      if (isset($attributes[['is_primary']])) {
        return (string) $id->Id;
      }
    }
    return (string) $ids[0]->Id;
  }

  /**
   * Adds the properties for a biomaterial in the XML loader.
   *
   * @param $attributes
   * @param $biomaterial_id
   * @param $insert_fields
   * @param $insert_cvalues
   */
  protected function add_xml_biomaterial_properties(
    $attributes,
    $biomaterial_id,
    $insert_fields,
    $insert_cvalues
  ) {

    $record = [
      'table' => 'biomaterial',
      'id' => $biomaterial_id,
    ];

    $options = [
      'update_if_present' => TRUE,
    ];

    // Iterate through each property.
    foreach ($attributes as $attr) {

      // Get the cvterm name.
      $attr_name = (string) $attr->attributes()->attribute_name;
      $value = (string) $attr;

      if ($attr_name == "biomaterial_provider") {
        continue;
      }

      // Insert into database.  Only insert into db if an existing CVTerm wasn't chosen.
      $cv_name = 'biomaterial_property';
      if (isset($insert_fields[$attr_name])) {

        $specified_prop_cv = $insert_fields[$attr_name]['cv_name'];
        $specified_prop_db = $insert_fields[$attr_name]['db_name'];
        $specified_prop_accession = $insert_fields[$attr_name]['cvt_accession'];
        $specified_prop_cvterm = $insert_fields[$attr_name]['cvt_name'];
        $cv_name = $specified_prop_cv;
        // We rename the attribute here, in case we chose a cvterm whose name is different from the attribute in the XML.
        $attr_name = $specified_prop_cvterm;

      }

      // If a cv wasnt specified, or if this term wasnt set in the fields array, need to insert into the biomaterial_property CV.
      else {
        if (!tripal_insert_cvterm([
          'name' => (string) $attr_name,
          'definition' => '',
          'cv_name' => 'biomaterial_property',
          'is_relationship' => 0,
          'db_name' => 'tripal',
        ], ['update_existing' => FALSE])) {
          $this->logMessage('Could not insert biomaterial attribute !name into the biomaterial_property controlled vocabulary table.', ['!name' => $attr_name], TRIPAL_ERROR);
          return FALSE;
        }
      }

      $cvalue_id = NULL;

      if (isset($insert_cvalues[$value])) {
        $cvalue_id = $insert_cvalues[$value];
      }

      // Insert the property into the biomaterialprop table.
      $property = [
        'type_name' => $attr_name,
        'cv_name' => $cv_name,
        'value' => $value,
        'cvalue_id' => $cvalue_id,
      ];
      chado_insert_property($record, $property, $options);
    }
  }

  /**
   * @param $file_path
   * @param $organism_id
   * @param $analysis_id
   * @param $file_type
   */
  protected function load_biosample_flat(
    $file_path,
    $insert_fields,
    $insert_cvalues
  ) {

    $organism_id = $this->organism_id;
    $analysis_id = $this->analysis_id;
    if (!is_readable($file_path)) {
      $this->logMessage("File is not readable or not present.",
        [], TRIPAL_ERROR);
      return;
    }

    $num_biosamples = 0;
    $cur_biosample = 0;

    $fileSize = filesize($file_path);
    if ($fileSize == 0) {
      $this->logMessage("File at !file_path is empty. Try again with a new file.",
        ['!file_path' => $file_path], TRIPAL_ERROR);
      return;
    }

    $extension = explode('.', $file_path);
    $extension = $extension[count($extension) - 1];

    // Defaults are for csv unless tsv selected.
    $separator = ",";
    $enclosure = '"';

    if ($extension == "tsv") {
      $separator = "\t";
      $enclosure = "\0";
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

    if ($num_biosamples == 0) {
      $message = "Wrong file format at !path. File must contain a column named 'sample_name'.
        Please try again with a file that contains at least one column named 'sample_name' in its header line, followed by lines of biosample data.";
      $this->logMessage($message, ['!path' => $file_path], TRIPAL_ERROR);
      return;
    }

    $fp = fopen($file_path, "r");

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

      $this->logMessage("There are multiple instance of the following header(s) in the biomaterial file:", [], TRIPAL_ERROR);
      foreach ($header_repeats as $repeat) {
        $this->logMessage("  !repeat", ['!repeat' => $repeat], TRIPAL_ERROR);
      }
      $this->logMessage("Please make sure there is only one instance of each header in the biomaterial file");
      return;
    }

    $biomaterial_names = [];
    $biomaterial_headers = [];
    $prop_headers = [];
    $acc_headers = [];

    for ($i = 0; $i < count($headers); $i++) {
      $header = trim(str_replace("*", "", $headers[$i]));
      if ($header == 'sample_name') {
        $biomaterial_headers['sample_name'] = $i;
      }
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
      $cur_biosample++;

      $this->logMessage('Loading Biosample !curr of !num', [
        '!curr' => $cur_biosample,
        '!num' => $num_biosamples,
      ]);

      $unique_name = '';
      $description = '';
      $biosourceprovider_id = '';
      $dbxref_id = '';

      $biomaterial_names[] = $line[$biomaterial_headers['sample_name']];

      // Gather the fields to insert into the biomaterial table.
      foreach ($biomaterial_headers as $name => $index) {
        if ($name == 'sample_name') {
          $unique_name = $line[$index];
        }
        if ($name == 'description') {
          $description = $line[$index];
        }
        if ($name == 'biomaterial_provider' and $line[$index] != "") {
          $biosourceprovider_id = tripal_biomaterial_create_biomaterial_contact($line[$index]);
        }
      }

      // Create the biomaterial based on the values given.
      $biomaterial_id = tripal_biomaterial_create_biomaterial($unique_name, $analysis_id, $organism_id, $biosourceprovider_id, $dbxref_id, $description, TRUE);

      // Insert database accessions.
      foreach ($acc_headers as $name => $index) {
        $url_name = '';
        $db_name = '';
        if ($name == 'sra_accession') {
          $url_name = 'sra';
          $db_name = 'NCBI SRA';
        }

        if ($name == 'biosample_accession') {
          $url_name = 'biosample';
          $db_name = 'NCBI Biosample';
        }
        if ($name == 'bioproject_accession') {
          $url_name = 'bioproject';
          $db_name = 'NCBI BioProject';
        }

        $db_id = tripal_biomaterial_create_ncbi_db($url_name, $db_name, '');

        // First check to see if there is a dbxref record in the database corresponding to the accession number from the file.
        $values = [
          'accession' => $line[$index],
          'db_id' => $db_id,
        ];
        $record = chado_select_record('dbxref', ['dbxref_id'], $values);
        if (! empty($record)) {
          $record = $record[0];
          $dbxref_id = $record->dbxref_id;
        } else {
          $record = chado_insert_record('dbxref', $values);
          $dbxref_id = $record['dbxref_id'];
        }

        // Insert into the biomaterial_dbxref table.
        $values = [
          'biomaterial_id' => $biomaterial_id,
          'dbxref_id' => $dbxref_id,
        ];

        $record = chado_select_record('biomaterial_dbxref', ['biomaterial_id'], $values);

        if (empty($record)) {
          $record = chado_insert_record('biomaterial_dbxref', $values);
        }
      }

      // Insert into the biomaterialprop table.
      foreach ($prop_headers as $name => $index) {

        // If we didnt choose an association in the form, use biomaterial_property.
        if (!key_exists($name, $insert_fields)) {

          // Get xml attributes.
          if (!tripal_insert_cvterm([
            'name' => $name,
            'definition' => '',
            'cv_name' => 'biomaterial_property',
            'is_relationship' => 0,
            'db_name' => 'tripal',
          ], ['update_existing' => FALSE])) {
            tripal_report_error('tripal_biomaterial_loader', TRIPAL_ERROR, 'Could not insert biomaterial attribute name into the controlled vocabulary table.');
            return;
          }

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
        else {
          $cv_name = "biomaterial_property";
          if ($insert_fields[$name]) {
            $cv_name = $insert_fields[$name];
          }
          // If the key exists in insert_fields array, the user chose a CV for this term, so we won't insert, but insert property with correct CV.
          $record = [
            'table' => 'biomaterial',
            'id' => $biomaterial_id,
          ];

          $cvalue_id = NULL;
          $cvalue_text = $line[$index];

          if ($insert_cvalues[$cvalue_text]) {
            $cvalue_id = $insert_cvalues[$cvalue_text];
          }

          $property = [
            'type_name' => $name,
            'cv_name' => $cv_name,
            'value' => $cvalue_text,
            'cvalue_id' => $cvalue_id,
          ];
          $options = [
            'update_if_present' => TRUE,
          ];
          chado_insert_property($record, $property, $options);
        }
      }
    }

    // Make sure there are not duplicate biomaterials.
    if (count($sample_repeats = array_diff_assoc($biomaterial_names, array_unique($biomaterial_names))) > 0) {
      // If (count($linepart) != count(array_unique($linepart))) {.
      $this->logMessage("There are multiple instances of the following sample_name(s) in the biomaterial file:", [], TRIPAL_ERROR);
      foreach ($sample_repeats as $repeat) {
        $this->logMessage("  !repeat", ['!repeat' => $repeat], TRIPAL_ERROR);
      }
      $this->logMessage("Please make sure there is only one instance of each sample_name in the biomaterial file.", [], TRIPAL_ERROR);
      return;
    }

    return TRUE;
  }

  /**
   * Get the filepath of the biomaterial file (local, upload, or upload
   * existing).
   *
   * @param $values
   *
   * @return null|string
   */
  private function findFile($values) {

    $file_path = NULL;

    if (isset($values['file_local'])) {
      $file_path = trim($values['file_local']);
    }
    if (isset($form_state['values']['file_upload'])) {
      $fid = trim($values['file_upload'][0]);
      if (!empty($fid)) {
        $file = file_load($fid);
        $file_path = base_path() . drupal_realpath($file->uri);
      }
    }

    if (isset($values['file_upload_existing'])) {
      $fid = $values['file_upload_existing'];
      if (!empty($fid)) {
        $file = file_load($fid);
        $file_path = base_path() . drupal_realpath($file->uri);
      }
    }
    return $file_path;
  }

  /**
   *
   */
  private function parse_xml_biomaterial_file($file_path) {

    $xml = $this->read_xml_file($file_path);

    if (!$xml) {
      return;
    }

    $num_biosamples = count($xml->BioSample);
    $biomaterial_names = [];

    // Make sure there are no duplicate biomaterials.
    for ($i = 0; $i < $num_biosamples; $i++) {
      $biomaterial_names[] = $xml->BioSample[$i]->Ids->Id[1];
    }

    $attribute_list = [];

    foreach ($xml->BioSample as $biosample) {

      // Dont get confused here-> we actually look for a node called "attributes" with child node "attribute".
      $attributes = $biosample->Attributes->Attribute;
      foreach ($attributes as $key => $attribute_object) {

        $attribute_name_index = (string) $attribute_object->attributes()->attribute_name;

        // The list of attribute names, ie "name = tissue", "display_name = Tissue".
        $attributes = $attribute_object->attributes();
        // The string value ie "leaf".
        $property_value = $attribute_object->__toString();
        $this_attribute = [];

        foreach ($attributes as $property_sub_name => $property_sub_value) {

          $value_string = (string) $property_sub_value;

          // The attribute has multiple names: machine readable, human readable.  let's pass all of them to our tool so that any and all matches are found.
          $this_attribute[$property_sub_name] = $value_string;
        }

        $attribute_list["attributes"][$attribute_name_index] = $this_attribute;
        $attribute_list["values"][] = $property_value;
      }
    }

    return $attribute_list;
  }

  /**
   * Reads the XML file and raises any errors for initial parsing.
   *
   * @param $file
   *
   * @return null|\SimpleXMLElement
   */
  private function read_xml_file($file) {
    $xml_raw = file_get_contents($file);

    if (!$xml_raw) {
      $this->logMessage('Unable to read XML file.', [], TRIPAL_ERROR);
    }
    $xml = simplexml_load_string($xml_raw);

    if (!$xml) {
      $message = "Error reading xml.\n";
      foreach (libxml_get_errors() as $error) {
        $message .= $error;
      }
      $this->logMessage($message, [], TRIPAL_ERROR);
      return NULL;
    }

    if (!$xml->BioSample) {
      $this->logMessage("XML must contain a BioSample tag.  Check your XML file and try again.", [], TRIPAL_ERROR);
      return NULL;
    }

    // Make sure there are not duplicate biomaterials.
    $biomaterial_names = [];
    for ($i = 0; $i < count($xml->Biosample); $i++) {
      $Ids = $xml->BioSample[$i]->Ids;
      $biomaterial_name = $this->find_primary_ID($Ids);
      $biomaterial_names[] = $biomaterial_name;
    }

    if (count($sample_repeats = array_diff_assoc($biomaterial_names, array_unique($biomaterial_names))) > 0) {
      $message = "There are multiple instance of the following " . "Sample name(s) in the biomaterial file:\n";
      foreach ($sample_repeats as $repeat) {
        $message .= "  " . $repeat . "\n";
      }
      $message .= "Please make sure there is only one instance of each Sample name in the biomaterial file.\n";
      $this->logMessage($message, [], TRIPAL_ERROR);
      return NULL;
    }

    return $xml;

  }

  // End of class.
}

/**
 * @param $file_path
 * @param $organism_id
 * @param $analysis_id
 *
 * @return array
 *
 *   This function goes through a flat file and will return the headers.
 */
function test_biosample_cvterms_flat(
  $file_path
) {
  $attribute_list = [];

  $num_biosamples = 0;
  $cur_biosample = 0;

  // Check if the file given is empty.
  $fileSize = filesize($file_path);
  if ($fileSize == 0) {

    tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR, "File at !file_path is empty. Try again with a new file.",
      ['!file_path' => $file_path]);
    return;
  }
  // Figure out CSV vs TSV.
  $extension = explode('.', $file_path);
  $extension = $extension[count($extension) - 1];

  if ($extension == "tsv") {
    $separator = "\t";
    $enclosure = "\0";
  }
  else {
    // Auto assume file is using csv format.
    $separator = ",";
    $enclosure = '"';
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

  // Print error message and exit if there's no biosample, or that there's no "sample_name" column in flat file.
  if ($num_biosamples == 0) {
    tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR, "Wrong file format at !file_path. File must contain a column named 'sample_name'.",
      ['!file_path' => $file_path]);
    return;
  }

  // Get the file pointer.
  $fp = fopen($file_path, "r");
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
    tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR, "The header !header is present more than once.  Please make sure there is only one instance of each header.",
      ['!header' => $header_repeats[0]]);
    return;
  }

  for ($i = 0; $i < count($headers); $i++) {
    $header = trim(str_replace("*", "", $headers[$i]));
    if (in_array($header, [
      'sample_name',
      'organism',
      'description',
      ' biomaterial_provider',
      'biomaterial_accession',
      'sra_accession',
      'bioproject_accession',
    ])) {
    }
    else {
      $attribute_list[]["attributes"] = $header;

      // TODO: this is where the flat file loader would support checking the cvalues as well.
      $attribute_list[]["values"] = NULL;

    }
  }
  return ($attribute_list);
}

/**
 * @param $form
 * @param $form_state
 *
 *   AJAX callback to update the cvterm configuration once a file is loaded.
 *
 * @return mixed
 */
function cvterm_validator_callback(&$form, &$form_state) {

  return $form['cvterms'];
}
