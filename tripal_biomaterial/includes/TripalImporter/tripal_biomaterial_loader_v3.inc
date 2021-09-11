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

  public static $upload_description = "Please upload an NCBI BioSample file.  This
    can be in XML with an .xml extension, or flat file format with a .tsv or .csv
    extension.<br><br> If loading a CSV/TSV flat file, the first line must be the
    column name.  The only field that is required to create a biosample is the name
    (column: sample_name). It is recommended that a description (column: description),
    biomaterial provider (column: biomaterial_provider), accession (column: biomaterial_accession),
    treatment (column: treatment), and tissue (column: tissue) also be provided.
.  <br>This loader will create dbxref records for the following
    column headers if present: biosample_accession, bioproject_accession, and
    sra_accession. Other accessions should be uploaded using a bulk loader template. ";

  public static $upload_title = 'Import new Biosamples/Biomaterials';

  public static $button_text = 'Import Biosamples';

  public static $use_button = FALSE;

  public static $methods = [
    'file_upload' => FALSE,
    'file_local' => FALSE,
    'file_remote' => FALSE,
  ];

  public static $file_required = FALSE;

  public static $use_analysis = FALSE;
  public static $require_analysis = FALSE;

  private $file_path = NULL;
  private $file_extension = '';
  private $flat_parsing = [
    "tsv" => [
      "separator" => "\t",
      "enclosure" => "\0",
    ],
    "csv" => [
      "separator" => ",",
      "enclosure" => '"',
    ]
  ];


  // This arrays is meant to help map column headers in the CSV/TSV files
  // provided by NCBI to the BioSample attributes or to map to other
  // terms.
  private $default_terms = [
    'biosample' => [
      'term' => 'local:biosample_accession',
      'type' => 'annotation',
    ],
    'sample_name' => [
      'term' => 'local:sample_name',
      'type' => 'annotation',
    ],
    'sample_title' => [
      'term' => 'local:sample_title',
      'type' => 'annotation',
    ],
    'organism' => [
      'term' => 'OBI:0100026',
      'type' => 'annotation',
    ],
    'geo_accession__exp_' => [
      'term' => 'NCBI_BioSample:geo_loc_exposure',
      'type' => 'annotation'
    ]
  ];

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {
    if (array_key_exists('triggering_element', $form_state) and
        $form_state['triggering_element']['#type'] == 'submit' and
        ($form_state['triggering_element']['#value'] == 'Next Step' or
    $form_state['triggering_element']['#value'] == 'Previous Step')) {
      $form_state['storage']['current_step'] = $form_state['triggering_element']['#name'];
    }

    $current_step = 'step1';
    if (array_key_exists('storage', $form_state)) {
      $current_step = $form_state['storage']['current_step'];
    }

    // Based on the current stage, return the correct form.
    switch ($current_step) {
      case 'step1':
        $form = $this->formStep1($form, $form_state);
        break;
      case 'step2':
        $form = $this->formStep2($form, $form_state);
        break;
      case 'step3':
        $form = $this->formStep3($form, $form_state);
        break;
    }

    return $form;

  }

  /**
   * Step1 form elements.
   */
  private function formStep1($form, &$form_state) {
    global $user;

    $class = get_called_class();

    // Now to add the file + description based on our class.
    $form['file'] = [
      '#type' => 'fieldset',
      '#title' => t($class::$upload_title),
      '#description' => t($class::$upload_description),
    ];

    $existing_files = tripal_get_user_uploads($user->uid, $class::$file_types);
    $fids = [];
    if (count($existing_files) > 0) {
      $fids = [0 => '--Select a file--'];
      foreach ($existing_files as $fid => $file) {
        $fids[$fid] = $file->filename . ' (' . tripal_format_bytes($file->filesize) . ') ';
      }
    }
    $form['file']['file_upload_existing'] = [
      '#type' => 'select',
      '#title' => t('Existing Files'),
      '#description' => t('You may select a file that is already uploaded.'),
      '#options' => $fids,
    ];

    $form['file']['file_upload'] = [
      '#type' => 'html5_file',
      '#title' => '',
      '#description' => 'Remember to click the "Upload" button below to send ' .
      'your file to the server.  This interface is capable of uploading very ' .
      'large files.  If you are disconnected you can return, reload the file and it ' .
      'will resume where it left off.  Once the file is uploaded the "Upload ' .
      'Progress" will indicate "Complete".  If the file is already present on the server ' .
      'then the status will quickly update to "Complete".',
      '#usage_type' => 'tripal_importer',
      '#usage_id' => 0,
      '#allowed_types' => $class::$file_types,
      '#cardinality' => $class::$cardinality,
    ];

    $form['file']['file_local'] = [
      '#title' => t('Server path'),
      '#type' => 'textfield',
      '#maxlength' => 5120,
      '#description' => t('If the file is local to the Tripal server please provide the full path here.'),
    ];

    $analyses = chado_get_analysis_select_options(FALSE);
    $form['analysis_id'] = [
      '#title' => t('Analysis'),
      '#type' => t('select'),
      '#description' => t('Choose the analysis to which the uploaded data will be associated. ' .
          'Why specify an analysis for a data load?  All data comes from some place, even if ' .
          'downloaded from a website. By specifying analysis details for all data imports it ' .
          'provides provenance and helps end user to reproduce the data set if needed. At ' .
          'a minimum it indicates the source of the data.'),
      '#required' => FALSE,
      '#options' => $analyses,
    ];


    $organisms = chado_get_organism_select_options(FALSE);
    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#required' => TRUE,
      '#default_value' => 0,
      '#description' => t('The organism from which the biosamples were
         collected.  Each upload must consist of samples from the same organism.'),
      '#options' => $organisms,
    ];


    $form['next_step'] = array(
      '#type' => 'submit',
      '#name' => 'step2',
      '#value' => 'Next Step',
      '#weight' => 100,
    );

    return $form;
  }

  /**
   *
   */
  private function formStep2($form, &$form_state) {
//     $form['biomaterial_cvterm_validator'] = [
//       '#type' => 'button',
//       '#value' => t('Check Biosamples'),
//       '#ajax' => [
//         'callback' => 'cvterm_validator_callback',
//         'wrapper' => 'cvterm_configuration_div',
//       ],
//       '#submit' => ['cvterm_validator_callback_submit'],
//     ];

    $form['attributes'] = [
      '#type' => 'item',
      '#title' => 'Instructions',
      '#markup' => t('All attributes from the input file for the biomaterial
         must be associated with a controlled
         vocabulary term. Use the form below to make the association.
         If an appropriate term exists and is not present you can add one
         !here.  You can find terms at the !EBI. If you cannot
         find an appropriate term, you can add one to the "local" vocabulary.
         However, best practice is to use existing terms if possible.
         If you leave any attribute without an association, then the property
         will be excluded and will not be visible to end-users.',
          [
            '!here' => l('here', 'http://localhost/admin/tripal/loaders/chado_vocabs/chado_cv/cvterm/add', ['attributes' => ['target' => '_blank']]),
            '!EBI' => l('EBI Ontology Lookup Service' ,'https://www.ebi.ac.uk/ols/index', ['attributes' => ['target' => '_blank']])
          ]) .
         '<p><i>Note:</i> Any attributes that do not get assigned a term
      will not be imported with the biomaterial</p>',
      '#weight' => -100,
    ];

    if ($this->file_extension != 'xml') {
      drupal_set_message(t('If you are importing a CSV from NCBI\'s SRA Run
        selector it will have attributes specific to a sequencing run that may not apply
        to the biomaterial.  Be sure not to set terms for run-specific attributes
        for a biomaterial.'), 'warning');
    }

    // For Ajax triggered form elements we don't want the fieldset to close
    // so we need to know the delta value of the triggering parent to
    // keep the fieldset open.
    $triggered_parent = NULL;
    if ($form_state['triggering_element']['#type'] == 'button' and
        $form_state['triggering_element']['#value'] == 'Lookup Term') {
      $triggered_parent = $form_state['triggering_element']['#parents'][0];
      $triggered_parent = preg_replace('/select_button/', '', $triggered_parent);
    }


    $this->findFile($form_state);
    $attributes = $this->getAttributes();
    $delta = 0;
    $has_sample_name = FALSE;
    foreach ($attributes as $attr_name) {

      // NCBI has changed the headers so to support consistency make
      // them all lower case with underscores separating non alphanumeric
      // characters
      $attr_name_adj = preg_replace('/[^\w]/', '_', strtolower($attr_name));

      if ($attr_name_adj == 'sample_name') {
        $has_sample_name = TRUE;
      }

      $default_term_name = '';
      $default_data_type = 0;
      $term_id = '';
      $term = NULL;

      // Get the value from the form state
      if (array_key_exists('values', $form_state) and array_key_exists('data_class' . $delta, $form_state['values'])) {
        $default_data_type = $form_state['values']['term_match' . $delta]['data_class' . $delta];
      }
      // Get the default value for this attribute if the the form is being
      // reloaded due to an error.
      else if ($form_state['triggering_element']['#type'] == 'submit' and
               $form_state['triggering_element']['#name'] == 'step3' and
               $form_state['triggering_element']['#value'] == 'Next Step') {
        // Do nothing, we can't get the $form_state['input'] from the API.
      }
      // Get the default value for this attribute if the form is being
      // reloaded by returning from step 3.
      elseif ($form_state['triggering_element']['#type'] == 'submit' and
              $form_state['triggering_element']['#name'] == 'step2' and
              $form_state['triggering_element']['#value'] == 'Previous Step') {
          if (array_key_exists('biomaterial_key.' . $attr_name_adj . '.id', $form_state['storage'])) {
            $default_data_type = $form_state['storage']['biomaterial_key.' . $attr_name_adj . '.class'];
            $default_term_id = $form_state['storage']['biomaterial_key.' . $attr_name_adj . '.id'];
          $default_term = chado_get_cvterm(['cvterm_id' => $default_term_id]);
          $default_term_name = $default_term->name;
        }
      }
      // Get the default value for the attribute if this is being reloaded
      // from an AJAX submit for the lookup form.
      elseif ($form_state['triggering_element']['#type'] == 'button' and
              $form_state['triggering_element']['#value'] == 'Lookup Term') {
        // Do nothing, let the API handle it.
      }
      // If we can't set the default value from a form resubmit then
      // we'll try the global drupal variable that has some defaults.
      else {
        // First see if the term is found in the NCBI biosample vocab.
        $term_id = 'NCBI_BioSample:' . $attr_name_adj;
        $term = chado_get_cvterm(['id' => $term_id]);
        if ($term) {
          $default_term_name = $term->name;
        }
        elseif (array_key_exists($attr_name_adj, $this->default_terms)) {
          $default_data_type = $this->default_terms[$attr_name_adj]['type'];
          $term_id = $this->default_terms[$attr_name_adj]['term'];
          $term = chado_get_cvterm(['id' => $term_id]);
          if ($term) {
            $default_term_name = $term->name;
          }
        }
      }

      $title = $attr_name;
      if ($term) {
        $title .= ' -- ' . $term_id;
      }

      // Add the lookup fieldset to the form.
      tripal_get_term_lookup_form($form, $form_state, $default_term_name,
          $title, '', FALSE,  '', $delta,
          'tripal_biomaterial_loader_form_ajax_callback');

      // Make sure the correct item is checked.
      if ($term) {
        $term_element_name = 'term-' . $term->cvterm_id . '-' . $delta;
        $form['term_match' . $delta]['terms_list' . $delta][$term_element_name]['#default_value'] = TRUE;
      }

      // Add a few more items to the fieldset.
      $form['term_match' . $delta]['data_class' . $delta] = [
        '#type' => 'select',
        '#options' => [
          0 => 'Use defaults',
          'quantitative' => 'Numerical',
          'ordinal' => 'Numeric ordinal (coded categorical)',
          'annotation' => 'Text: annotation text (not filterable)',
          'categorical' => 'Text: categorical variable name',
        ],
        '#description' => t('We need to know the type of
           data for each attribute because elements with numerical or categorical
           are stored differently. This loader will do its best to determine the type but
           you can specify the type here if needed.'),
        '#default_value' => $default_data_type,
      ];

      // Make the form element collapsed.
      $form['term_match' . $delta]['#collapsible'] = TRUE;
      if ($triggered_parent !== "$delta") {
        $form['term_match' . $delta]['#collapsed'] = TRUE;
      }
      $delta++;
    }

    if (!$has_sample_name) {
      drupal_set_message(t('The attribute with the sample name could not be
        identified in the input file. If a sample name is not provided the
        loader cannot continue. Please set at least one attribute below to
        use the "local:sample_name" term.'), 'error');
    }

    $form['next_step'] = array(
      '#type' => 'submit',
      '#name' => 'step3',
      '#value' => 'Next Step',
      '#weight' => 100,
    );

    return $form;
  }

  /**
   *
   */
  private function formStep3($form, &$form_state) {
    $form['next_step'] = array(
      '#type' => 'submit',
      '#name' => 'done',
      '#value' => 'Submit',
      '#weight' => 100,
    );
    return $form;
  }

  /**
   * See TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {
    $forward = TRUE;

    // If the submit was clicked, iterate the stage.
    $next_step = 'step1';
    if ($form_state['triggering_element']['#type'] == 'submit') {
      $next_step = $form_state['triggering_element']['#name'];
    }

    if (array_key_exists('triggering_element', $form_state) and
        $form_state['triggering_element']['#type'] == 'submit') {
          if ($form_state['triggering_element']['#value'] == 'Previous Step') {
            $forward = FALSE;
          }
          $next_step = $form_state['triggering_element']['#name'];
        }

        if ($next_step == 'step2') {
          $form_state['rebuild'] = TRUE;
          if ($forward) {
            $this->formValidateStep1($form, $form_state);
          }
        }
        if ($next_step == 'step3') {
          $form_state['rebuild'] = TRUE;
          if ($forward) {
            $this->formValidateStep2($form, $form_state);
          }
        }
        if ($next_step == 'done') {
          $this->formValidateStep3($form, $form_state);
        }
  }

  /**
   *
   */
  private function formValidateStep1($form, &$form_state) {

    $file_local = NULL;
    $file_upload = NULL;
    $file_existing = NULL;
    $organism_id = $form_state['values']['organism_id'];
    $analysis_id = $form_state['values']['analysis_id'];

    // Get the form values for the file.
    $file_local = trim($form_state['values']['file_local']);
    if ($file_local) {
      // check to see if the file is located local to Drupal
      $file_local = trim($file_local);
      $dfile = $_SERVER['DOCUMENT_ROOT'] . base_path() . $file_local;
      if (!file_exists($dfile)) {
        // if not local to Drupal, the file must be someplace else, just use
        // the full path provided
        $dfile = $file_local;
      }
      if (!file_exists($dfile)) {
        form_set_error('file_local', t("Cannot find the file on the system. Check that the file exists or that the web server has permissions to read the file."));
      }
    }

    $file_upload = trim($form_state['values']['file_upload']);
    $file_existing = trim($form_state['values']['file_upload_existing']);

    // The user must provide at least an uploaded file or a local file path.
    if (!$file_upload and !$file_local and !$file_existing) {
      form_set_error('', t("You must provide a file."));
    }

    // Check if the file given is empty.
    $this->findFile($form_state);
    $fileSize = filesize($this->file_path);
    if ($fileSize == 0) {
      form_set_error('', t("The provided file is empty"));
    }

    // Put the values from this step of the form into storage.
    $form_state['storage']['file_local'] = $file_local;
    $form_state['storage']['file_upload'] = $file_upload;
    $form_state['storage']['file_upload_existing'] = $file_existing;
    $form_state['storage']['organism_id'] = $organism_id;
    $form_state['storage']['analysis_id'] = $analysis_id;
  }

  /**
   *
   */
  private function formValidateStep2($form, &$form_state) {
    $delta = 0;
    $this->findFile($form_state);
    $attributes = $this->getAttributes();
    foreach ($this->$attributes as $attr_name) {
      $selected_term = tripal_get_term_lookup_form_result($form, $form_state,'',$delta);
      if ($selected_term) {
        $cvterm_id = $selected_term[0]->cvterm_id;
        $form_state['storage']['biomaterial_key.' . $attr_name . '.id'] = $cvterm_id;
      }
      else {
        $form_state['storage']['biomaterial_key.' . $attr_name . '.id'] = NULL;
      }
      $form_state['storage']['biomaterial_key.' . $attr_name . '.class'] = $form_state['values']['data_class' . $delta];
      $delta++;
    }
  }
  /**
   *
   */
  private function formValidateStep3($form, &$form_state) {
    // Put all of the values in storage back into the values array for
    // job submission.
    $this->findFile($form_state);
    $attributes = $this->getAttributes();
    $delta = 0;
    foreach ($this->$attributes as $attr_name) {
      if (array_key_exists('biomaterial_key.' . $attr_name . '.id', $form_state['storage'])) {
        $form_state['values']['biomaterial_key.' . $attr_name . '.id'] = $form_state['storage']['biomaterial_key.' . $attr_name . '.id'];
      }
      if (array_key_exists('biomaterial_key.' . $attr_name . '.class', $form_state['storage'])) {
        $form_state['values']['biomaterial_key.' . $attr_name . '.class'] = $form_state['storage']['biomaterial_key.' . $attr_name . '.class'];
      }
      if (array_key_exists('biomaterial_key.' . $attr_name . '.missing', $form_state['storage'])) {
        $form_state['values']['biomaterial_key.' . $attr_name . '.missing'] = $form_state['storage']['biomaterial_key.' . $attr_name . '.missing'];
      }
      $delta++;
    }
    $form_state['values']['biomaterial_file_local'] = $form_state['storage']['file_local'];
    $form_state['values']['biomaterial_file_upload'] = $form_state['storage']['file_upload'];
    $form_state['values']['biomaterial_file_upload_existing'] = $form_state['storage']['file_upload_existing'];
    $form_state['values']['organism_id'] = $form_state['storage']['organism_id'];
    $form_state['values']['analysis_id'] = $form_state['storage']['analysis_id'];
  }

  private function getAttributes() {
    if ($this->file_extension == 'xml') {
      return $this->getXMLAttributes();
    }
    else {
      return $this->getCSVAttributes();
    }
  }

  private function getXMLAttributes() {

  }

  private function getCSVAttributes() {

    // Open the file and get the first line. It should be the header line.
    $attributes = [];
    $separator = $this->flat_parsing[$this->file_extension]['separator'];
    $enclosure = $this->flat_parsing[$this->file_extension]['enclosure'];
    $fp = fopen($this->file_path, "r");
    if ($fp) {
      $attributes = fgetcsv($fp, 0, $separator, $enclosure);
    }
    return $attributes;

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
      $extension = pathinfo($this->arguments['files'][0]['file_remote'], PATHINFO_EXTENSION);
    }
    else {
      $extension = pathinfo($file_path, PATHINFO_EXTENSION);
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

    $extension = pathinfo($file_path, PATHINFO_EXTENSION);

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
        if (preg_match("/(sample[_\s]name)/i", $field)) {
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
        if (preg_match("/(sample[_\s]name)/i", $field)) {
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
      $header = preg_replace('/[^\w]/', '_', strtolower($headers[$i]));
      //$header = trim(str_replace("*", "", $headers[$i]));
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

      $this->logMessage('Loading Biosample !name, !curr of !num', [
        '!name' => $line[$biomaterial_headers['sample_name']],
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
          if ($insert_fields[$name]->cvt_name) {
            $cv_name = $insert_fields[$name]->cvt_name;
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
  private function findFile($form_state) {
    if (array_key_exists('values', $form_state) and
        array_key_exists('file_local', $form_state['values'])) {
      $values = $form_state['values'];
    }
    else {
      $values = $form_state['storage'];
    }

    if (!empty($values['file_local'])) {
      $this->file_path = trim($values['file_local']);
    }
    if (!empty($values['file_upload'])) {
      $fid = trim($values['file_upload']);
      if (!empty($fid)) {
        $file = file_load($fid);
        $this->file_path = base_path() . drupal_realpath($file->uri);
      }
    }

    if (!empty($values['file_upload_existing'])) {
      $fid = $values['file_upload_existing'];
      if (!empty($fid)) {
        $file = file_load($fid);
        $this->file_path = base_path() . drupal_realpath($file->uri);
      }
    }

    if ($this->file_path) {
      $this->file_extension = pathinfo($this->file_path, PATHINFO_EXTENSION);
    }
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
    tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR,
        "File at !file_path is empty. Try again with a new file. file_path: !file_path",
        ['!file_path' => $file_path],
        ['drupal_set_message' => TRUE]);
   #$this->logMessage("File at !file_path is empty. Try again with a new file.",
   #   ['!file_path' => $file_path], TRIPAL_ERROR);
    return;
  }

  // Figure out CSV vs TSV.
  $extension = pathinfo($file_path, PATHINFO_EXTENSION);

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
      if (preg_match("/(sample[_\s]name)/i", $field)) {
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
    #$this->logMessage("Wrong file format at !file_path. File must contain a column named 'sample_name'.",
    #  ['!file_path' => $file_path], TRIPAL_ERROR);
    tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR,
        "Wrong file format of !file_path. The file must contain a column named 'sample_name'",
        ['!file_path' =>  pathinfo($file_path, PATHINFO_FILENAME)],
        ['drupal_set_message' => TRUE]);
    return;
  }


  // Get the file pointer.
  $fp = fopen($file_path, "r");
  $nLineHeader = 0;
  while ($line = fgetcsv($fp, 0, $separator, $enclosure)) {
    $nLineHeader++;
    foreach ($line as $field) {
      if (preg_match("/(sample[_\s]name)/i", $field)) {
        break 2;
      }
    }
  }
  $headers = $line;

  // Make sure there are not duplicate biomaterial headers.
  if (count($header_repeats = array_diff_assoc($headers, array_unique($headers))) > 0) {
    tripal_report_error('tripal_analysis_expression', TRIPAL_ERROR,
      "The header !header is present more than once.  Please make sure there is only one instance of each header.",
      ['!header' => $header_repeats[0]],
      ['drupal_set_message' => TRUE]);
    return;
  }

  // Get the column headers that need CVterms. We will exclude those
  // that we already know the terms for.
  $attribute_list["attributes"] = [];
  for ($i = 0; $i < count($headers); $i++) {
    $header = preg_replace('/[^\w]/', '_', strtolower($headers[$i]));
    //$header = trim(str_replace("*", "", $headers[$i]));
    if (in_array($header, [
      'sample_name',
      'organism',
      'description',
      'biomaterial_provider',
      'biomaterial_accession',
      'sra_accession',
      'bioproject_accession',
    ])) {
    }
    else {
      $attribute_list["attributes"][$header] = $header;
    }
  }

  // Fill the values into the attribute_list array from lines in the file. Starting from the next line after the header
//  TODO: the following code adds every value of every column as a potential term
// which from aN NCBI SRA file that overruns the page!
//   $attribute_list["values"] = [];
//   $tmpNumLine = 0;
//   while ($line = fgetcsv($fp, 0, $separator, $enclosure)) {
//     $tmpNumLine++;
//     if($tmpNumLine > $nLineHeader){
//       $attribute_list["values"] = array_merge($attribute_list["values"], $line);
//     }
//   }
//   $attribute_list["values"] = array_unique($attribute_list["values"]);

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


/**
 * Implements an AJAX callback for the tripal_chado_vocab_select_term_form.
 *
 * @ingroup tripal_terms_api
 */
function tripal_biomaterial_loader_form_ajax_callback($form, $form_state) {
  $ajax_wrapper_id = $form_state['triggering_element']['#ajax']['wrapper'];

  $delta = $form_state['storage'][$ajax_wrapper_id]['term_match_delta'];
  return $form['term_match' . $delta];
}