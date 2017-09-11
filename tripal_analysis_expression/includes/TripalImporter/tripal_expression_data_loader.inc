<?php

class tripal_expression_data_loader extends TripalImporter{

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
  public static $name = 'Chado Expression Data Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'Chado Expression Data Loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Import expression data for a single or multiple biomaterial/biosample and an analysis';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['csv', 'tsv', 'txt'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Expression data can be loaded from two format types. Select the column format for files that have two columns - transcript id and expression value. Select the matrix format for files that specify biosample by column and transcript by row. If "Column Format" is selected, the name of the column file will be taken as the biomaterial name. It is recommended to avoid the use of white space in column file names and in biomaterial names.)';

  /**
   * The title that should appear above the upload button.
   */
  public static $upload_title = 'Upload expression data';

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
  public static $button_text = 'Import expression data';

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

    $default_time = REQUEST_TIME;
    $year = format_date($default_time, 'custom', 'Y');
    $month = format_date($default_time, 'custom', 'n');
    $day = format_date($default_time, 'custom', 'j');

    // get a list of organisms for the user to choose
    $sql = "SELECT * FROM {organism} ORDER BY genus, species";
    $results = chado_query($sql);
    $organisms = [];
    foreach ($results as $org) {
      $organisms[$org->organism_id] = $org->common_name;
    }

    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#required' => TRUE,
      '#description' => t('The organism from which the biosamples were collected.'),
      '#options' => $organisms,
    ];

    // At this time the analysis module has the extra fields.
    // These fields do not appear to do anything for the analysis module.
    // The following three form fields prevent an error form appearing
    // when the expression biomaterial is submitted.
    $form['removed'] = [
      '#type' => 'value',
      '#default_value' => '',
    ];
    $form['num_new'] = [
      '#type' => 'value',
      '#default_value' => '',
    ];
    $form['analysis_type'] = [
      '#type' => 'value',
      '#default_value' => '',
    ];

    //protocols, assays, etc go here

    $form['experimental_design'] = [
      '#title' => t('Experimental Design'),
      '#type' => 'fieldset',
      '#description' => t('Describe the parameters from which the expression data was generated.'),
      '#attributes' => ['id' => 'expression-extra-settings'],
      '#weight' => 1,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ];

    // Obtain the list of contacts from the chado contact table.
    $sql = 'SELECT
            contact_id, name 
          FROM {contact} 
          ORDER BY name';
    $results = chado_query($sql);
    $contacts = [];
    while ($contact = $results->fetchObject()) {
      $contacts[$contact->contact_id] = $contact->name;
    }

    $form['choose_contact'] = [
      '#type' => 'value',
      '#value' => $contacts,
    ];

    $lt_message = tripal_set_message(t('If the biomaterial provider is not present in the dropdown list, ') . l(t('add a new contact.'), 'node/add/chado-contact') . t(' A biomaterial provider is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

    $form['experimental_design']['biosourceprovider_id'] = [
      '#title' => t('Biomaterial Provider'),
      '#type' => 'select',
      '#empty_value' => '',
      '#description' => t('The contact who provided the biomaterial. If the biomaterial is already') . (' present in the database, the biomaterial provider selected here will') . (' overwrite the biomaterial provider set for the biomaterial in the database.'),
      '#options' => $form['choose_contact']['#value'],
      '#suffix' => $lt_message,
    ];

    $lt_message = tripal_set_message(t('If the array design does not appear in this list, ') . l(t('add a new arraydesign.'), 'node/add/chado-arraydesign') . t(' If an array design is not selected, then a generic array design will be created for the analysis.'), TRIPAL_INFO, ['return_html' => TRUE]);

    $arraydesigns = tripal_get_arraydesign_select_options(TRUE);

    $form['experimental_design']['arraydesign_id'] = [
      '#title' => t('Array Design'),
      '#type' => 'select',
      '#description' => t('The array design associated with this analysis. This is not required if the experimental') . t(' data was gathered from next generation sequencing methods. If an array design is not') . t(' selected, a generic array design will be used as a place filler.'),
      '#options' => $arraydesigns,
      '#suffix' => $lt_message,
    ];
    //
    $form['experimental_design']['acquisition_details'] = [
      '#title' => t('Acquisition Details'),
      '#type' => 'fieldset',
      '#description' => t('Describe the details of the acquisition process. The acquisition is the process') . (' of scanning the experimental data.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 1,
    ];

    $form['experimental_design']['acquisition_details']['acquisitiondate'] = [
      '#type' => 'date',
      '#title' => t('Date Acquisition Run'),
      '#description' => t('If no date is provided, then the current date will be used.'),
      '#required' => TRUE,
      '#default_value' => [
        'year' => $year,
        'month' => $month,
        'day' => $day,
      ],
    ];

    $form['experimental_design']['acquisition_details']['acquisition_uri'] = [
      '#type' => 'textfield',
      '#title' => t('Acquisition URI'),
      '#description' => t('URI location that describes the acquisition.'),
    ];

    $lt_message = tripal_set_message(t('If the acquisition protocol is not present in the dropdown list, ') . l(t('add a new protocol.'), 'node/add/chado-protocol') . t(' An acquisition protocol is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

    $acquisitions = tripal_get_protocol_select_options(TRUE, 'Acquisition Protocol');

    $form['experimental_design']['acquisition_details']['acquisition_protocol_id'] = [
      '#title' => t('Acquisition Protocol'),
      '#type' => 'select',
      '#description' => t('The protocol associated with this acquisition.'),
      '#options' => $acquisitions,
      '#suffix' => $lt_message,
    ];

    $form['experimental_design']['quantification_details'] = [
      '#title' => t('Quantification Details'),
      '#type' => 'fieldset',
      '#description' => t('Describe the details of the quantification process. The quantification is the process of transforming raw expression data into numeric data.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 1,
    ];

    $form['experimental_design']['quantification_details']['quantificationdate'] = [
      '#type' => 'date',
      '#title' => t('Date Quantification Run'),
      '#description' => t('If no date is provided, then the current date will be used.'),
      '#required' => TRUE,
      '#default_value' => [
        'year' => $year,
        'month' => $month,
        'day' => $day,
      ],
    ];

    $form['experimental_design']['quantification_details']['quantification_uri'] = [
      '#type' => 'textfield',
      '#title' => t('Quantification URI'),
      '#description' => t('URI location that describes the quantification.'),
    ];

    $lt_message = tripal_set_message(t('If the quantification operator is not present in the dropdown list, ') . l(t('add a new contact.'), 'node/add/chado-contact') . t('A quantication operator is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

    //     Obtain the list of contacts from the chado contact table.
    $sql = 'SELECT
              contact_id, name
            FROM {contact}
            ORDER BY name';
    $results = chado_query($sql);
    $contacts = [];
    while ($contact = $results->fetchObject()) {
      $contacts[$contact->contact_id] = $contact->name;
    }

    $form['choose_contact'] = [
      '#type' => 'value',
      '#value' => $contacts,
    ];

    $form['experimental_design']['quantification_details']['quantification_operator_id'] = [
      '#title' => t('Quantification Operator'),
      '#type' => 'select',
      '#empty_value' => '',
      '#description' => t('The quantification operator. If the operator does not appear in this list, ') . l(t('add a new contact.'), 'node/add/chado-contact') . t(' An operator is not required.'),
      '#options' => $form['choose_contact']['#value'],
      '#suffix' => $lt_message,
    ];

    $lt_message = tripal_set_message(t('If the quantification protocol is not present in the dropdown list, ') . l(t('add a new protocol.'), 'node/add/chado-protocol') . t(' A quantification protocol is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

    $quantifications = tripal_get_protocol_select_options(TRUE, 'Quantification Protocol');

    $form['experimental_design']['quantification_details']['quantification_protocol_id'] = [
      '#title' => t('Quantification Protocol'),
      '#type' => 'select',
      '#description' => t('The protocol associated with this quantification.'),
      '#options' => $quantifications,
      '#suffix' => $lt_message,
    ];

    $form['filetype'] = [
      '#type' => 'radios',
      '#title' => t('Source File Type'),
      '#description' => t('Data can be loaded from two format types. Select the column format for files that have two columns - transcript id and expression value. Select the matrix format for files that specify biosample by column and transcript by row. If "Column Format" is selected, the name of the column file will be taken as the biomaterial name. It is recommended to avoid the use of white space in column file names and in biomaterial names.'),
      '#options' => [
        'col' => t('Column Format'),
        'mat' => t('Matrix Format'),
      ],
      '#required' => TRUE,
    ];

    $form['feature_uniquenames'] = [
      '#type' => 'radios',
      '#title' => t('Name Match Type'),
      '#description' => t('Expression data can be associated with features via the feature name or the feature unique name.'),
      '#options' => [
        'name' => t('Name'),
        'uniq' => t('Unique name'),
      ],
      '#required' => TRUE,
    ];

    $form['fileext'] = [
      '#type' => 'textfield',
      '#title' => t('File Type Suffix'),
      '#description' => t('Select the file extension for the file(s) to be loaded into Chado. Do not include the ".". It is not necessary to specifiy a file extension for a matrix file.'),
    ];

    $form['re_start'] = [
      '#type' => 'textfield',
      '#title' => t('Regex for Start of Data'),
      '#description' => t('If the expression file to be loaded has a header text, use a regular expression to describe the line that occurs before the start of the expression data. If the file has no header text, this line may be left blank.'),
    ];

    $form['re_end'] = [
      '#type' => 'textfield',
      '#title' => t('Regex for End of Data'),
      '#description' => t('If the expression file to be loaded has a footer text, use a regular expression to describe the line that occurs after the end of the expression data. If the file has no footer text, this line may be left blank.'),
    ];

    //$form['create_biosamples'] = [
    //  '#type' => 'checkbox',
    //  '#title' => t('Create New Biosamples (Caution!  Read below!)'),
    //  '#description' => t('Each expression set must be associated with a biosample.  We strongly recommend creating descriptive biosamples on your site or, ideally, in the NCBI Biosample database and importing the resulting xml into Chado.  Checking the box above will create a new biosample if the name does not match any records in the database.'),
    //];
    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */

  public function formValidate($form, &$form_state) {

    dpm($form_state['values']);

    $filetype = $form_state['values']['filetype'];
    $fileext = $form_state['values']['fileext'];
    $re_start = $form_state['values']['re_start'];
    $re_end = $form_state['values']['re_end'];

    // Validate the regular expressions if provided.
    if ($re_start and @preg_match('/' . $re_start . '/', NULL) === FALSE) {
      form_set_error('re_start', 'Invalid regular expression for the start of expression data: ' . $re_start);
    }
    if ($re_end and @preg_match('/' . $re_end . '/', NULL) === FALSE) {
      form_set_error('re_end', 'Invalid regular expression for the end of expression data: ' . $re_end);
    }
    if (empty($fileext) and $filetype === 'col') {
      form_set_error('fileext', 'Please specify a file extension for the column files to be loaded.');
    }
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    //Retrieve arguments
    $arguments = $this->arguments['run_args'];
    $filepath = $this->arguments['file']['file_path'];
    $fileext = $arguments['file_ext'];
    $filetype = $arguments['filetype'];
    $re_start = $arguments['re_start'];
    $re_stop = $arguments['re_stop'];
    $organism_id = $arguments['organism_id'];
    $analysis_id = $arguments['analysis_id'];
    $feature_uniquenames = $arguments['feature_uniquenames'];
    $arraydesign_id = $arguments['arraydesign_id'];
    $acquisitiondate = $arguments['acquisitiondate'];
    $create_biosamples = $arguments['create_biosamples'];
    $quantificationdate = $arguments['quantificationdate'];

    //missing
    // $assaydate = $arguments['assaydate'];

    // $contact_id = $arguments['contact_id'];

    //
    //$record = ['table' => 'analysis', 'id' => $node->analysis_id];
    //
    ////insert properties
    ////note this is a subset of properties from the original, and im also not sure that we still need it
    //chado_insert_property($record, [
    //  'type_name' => 'analysis_expression_filetype',
    //  'cv_name' => 'tripal',
    //  'value' => trim($node->filetype),
    //]);
    //chado_insert_property($record, [
    //  'type_name' => 'analysis_expression_submit_job',
    //  'cv_name' => 'tripal',
    //  'value' => trim($node->submit_job),
    //]);
    //chado_insert_property($record, [
    //  'type_name' => 'analysis_expression_fileext',
    //  'cv_name' => 'tripal',
    //  'value' => trim($node->fileext),
    //]);
    //chado_insert_property($record, [
    //  'type_name' => 'analysis_expression_filepath',
    //  'cv_name' => 'tripal',
    //  'value' => trim($node->filepath),
    //]);
    //chado_insert_property($record, [
    //  'type_name' => 'analysis_expression_re_start',
    //  'cv_name' => 'tripal',
    //  'value' => trim($node->re_start),
    //]);
    //chado_insert_property($record, [
    //  'type_name' => 'analysis_expression_re_end',
    //  'cv_name' => 'tripal',
    //  'value' => trim($node->re_end),
    //]);
    //chado_insert_property($record, [
    //  'type_name' => 'analysis_expression_feature_uniquenames',
    //  'cv_name' => 'tripal',
    //  'value' => trim($node->feature_uniquenames),
    //]);

    $this->tripal_expression_loader($organism_id, $analysis_id, $contact_id, $arraydesign_id, $assaydate, $acquisitiondate, $quantificationdate, $filepath, $fileext, $filetype, $re_start, $re_stop, $feature_uniquenames, $create_biosamples);
  }

  /**
   * Implements the function that will called as a tripal job
   *  to load expression data.
   *
   * @param $organism_id
   *  The organism_id of the organism from which the biomaterial was gathered.
   *
   * @param $analysis_id
   *  The analysis_id of the analysis: expression from which the expression
   *   loader is being called.
   *
   * @param $contact_id
   *  The contact_id of the contact that provided the biomaterial. This
   *   paramter
   *   is optional.
   *
   * @param $arraydesign_id
   *  The arraydesign_id of the arraydesign associated with the experiment.
   *   This
   *   parameter is optional.
   *
   * @param $assay_id
   *  The assay_id of the assay associated with the experiment. This parameter
   *   is optional.
   *
   * @param $acquisition_id
   *  The acquisition_id of the acquisition associated with the experiment.
   *   This
   *   parameter is optional.
   *
   * @param $quantificatin_id
   *  The quantification_id of the quantification associated with the
   *   experiment. This parameter is optional.
   *
   * @param $filepath
   *  The file path to the expression file/s to be loaded.
   *
   * @param $fileext
   *  The file extension of the expression files to be loaded.
   *
   * @param $filetype
   *  The file type can either by mat for matrix or col for column.
   *
   * @param $create_biosamples
   * Allow creation of new biosamples.  Defaults to false: we want users to
   *   load in annotated biosamples instead.
   *
   * @ingroup tripal_analysis_expression
   */

  function tripal_expression_loader(
    $organism_id,
    $analysis_id,
    $contact_id,
    $arraydesign_id,
    $assaydate,
    $acquisitiondate,
    $quantificationdate,
    $filepath,
    $fileext,
    $filetype,
    $re_start,
    $re_stop,
    $feature_uniquenames,
    $create_biosamples
  ) {


    //these should be passed in parameters but are optional and apparently not used.
    //BC
    $assay_id = NULL;
    $acquisition_id = NULL;
    $quantification_id = NULL;

    $features = [];

    $uniq_name = get_unique_name($organism_id, $analysis_id);

    $transaction = db_transaction();

    try {
      // Create the array design.
      $arraydesign_id = $arraydesign_id ? $arraydesign_id : create_generic_arraydesign(create_generic_contact());
      $re_ext = '/^.+\.(' . $fileext . ')/';

      // A matrix file.
      if ($filetype == 'mat' and is_file($filepath) and preg_match($re_ext, $filepath)) {

        tripal_expression_load_mat_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
      }

      // The column format.
      elseif ($filetype == 'col' and is_file($filepath) and preg_match($re_ext, $filepath)) {
        $features = tripal_expression_load_col_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
      }
      else { // Multiple files in the column format.
        $dir_handle = @opendir($filepath) or die("Unable to open $filepath");
        $total_files = count(glob($filepath . '/*.' . $fileext));
        $num_file = 0;

        // Load all files with of type $fileext in the directory.
        while ($file = readdir($dir_handle)) {
          if (preg_match('/(.+)\.' . $fileext . '$/', $file)) {
            print "File " . ($num_file + 1) . " of $total_files: $file                                       \n";
            $new_features = tripal_expression_load_col_file($filepath . '/' . $file, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
            $num_file++;
            $old_features = $features;
            $features = (array_unique(array_merge($old_features, $new_features)));
          }
        }
      }

      // Associate features with an analysis.
      foreach ($features as $feature_id) {
        $analysisfeature_id = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id', [
          ':analysis_id' => $analysis_id,
          ':feature_id' => $feature_id,
        ])->fetchField();
        if (!$analysisfeature_id) {
          $analysisfeature = chado_insert_record('analysisfeature', [
            'analysis_id' => $analysis_id,
            'feature_id' => $feature_id,
          ]);
          if ($analysisfeature == FALSE) {
            print "ERROR: Unable to write to analysisfeature table\n";
            exit;
          }
        }
      }
    } catch (Exception $e) {
      print "\n";
      watchdog_exception('T_expression_load', $e);
      $transaction->rollback();
      print "FAILED: Rolling back database changes...\n";
    }
    print "\nDone.\n";

    return;
  }

  function get_unique_name($organism_id, $analysis_id) {

    $uniq_string = " from ";
    $sql = "SELECT common_name FROM {organism} WHERE organism_id = :organism_id";
    $uniq_string .= chado_query($sql, [':organism_id' => $organism_id])->fetchField();
    $uniq_string .= " for ";
    $sql = "SELECT name FROM {analysis} WHERE analysis_id = :analysis_id";
    $uniq_string .= chado_query($sql, [':analysis_id' => $analysis_id])->fetchField();
    print $uniq_string;
    return $uniq_string;
  }

  /**
   *
   * Implements the function that will parse an expression file with a matrix
   * file.
   *
   * @param $organism_id
   *  The organism_id of the organism from which the biomaterial was gathered.
   *
   * @param $analysis_id
   *  The analysis_id of the analysis: expression from which the expression
   *   loader is being called.
   *
   * @param $contact_id
   *  The contact_id of the contact that provided the biomaterial. This
   *   paramter
   *   is optional.
   *
   * @param $arraydesign_id
   *  The arraydesign_id of the arraydesign associated with the experiment.
   *   This
   *   parameter is optional.
   *
   * @param $assay_id
   *  The assay_id of the assay associated with the experiment. This parameter
   *   is optional.
   *
   * @param $acquisition_id
   *  The acquisition_id of the acquisition associated with the experiment.
   *   This
   *   parameter is optional.
   *
   * @param $quantificatin_id
   *  The quantification_id of the quantification associated with the
   *   experiment. This parameter is optional.
   *
   * @param $filepath
   *  The file path to the expression file/s to be loaded.
   *
   * @param $fileext
   *  The file extension of the expression files to be loaded.
   *
   * @ingroup tripal_analysis_expression
   */

  function tripal_expression_load_mat_file(
    $filepath,
    $fileext,
    $arraydesign_id,
    $organism_id,
    $analysis_id,
    $contact_id,
    $assay_id,
    $acquisition_id,
    $quantification_id,
    $re_start,
    $re_stop,
    $uniq_name,
    $feature_uniquenames
  ) {

    $data_ln = 0;
    $mat_fp = fopen($filepath, 'r');
    $num_col = 0;
    $mat_head = 0;
    $num_features = 0;
    $num_lines = 0;
    $cur_feature = 1;
    $num_biomaterials = 0;

    $quant_array = [];
    $feature_array = [];

    // Count the number of features in the file.
    // Also make sure there are no duplicate features.

    // Ignore the feature footer.
    while ($line = fgets($mat_fp)) {
      $num_lines++;
      if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
        $data_ln = 0;
      }
      // Grab data lines. Ingore the header.
      if ($data_ln == 1 or !$re_start) {
        if ($mat_head == 0) {
          $num_features++;
          $mat_head = 1;
          continue;
        }
        elseif ($mat_head == 1) {
          $num_features++;
          preg_match("/^(.+?)\t/", $line, $match);
          $feature_array[] = trim($match[1]);
        }
      }
      // Only capture lines after the header.
      if (preg_match('/' . $re_start . '/', $line) and $re_start) {
        $data_ln = 1;
      }
    }

    // Make sure there not duplicate feature names.
    if (count($feature_repeats = array_diff_assoc($feature_array, array_unique($feature_array))) > 0) {
      //if (count($linepart) != count(array_unique($linepart))) {
      print "ERROR: There are multiple instance of the following " . "feature(s) in the expression file:\n";
      foreach ($feature_repeats as $repeat) {
        print "  " . $repeat . "\n";
      }
      print "Please make sure there is only one instance of each " . "feature in the expression file\n";
      exit;
    }

    // Get rid of the feature_array.
    unset($feature_array);

    //fclose($mat_fp);
    // Rewind the file.
    rewind($mat_fp);
    $mat_head = 0;

    $mat_fp = fopen($filepath, 'r');

    $set_fp = '';

    $load_limit = 500;

    // Calculate interval length.
    $int_length = .01 * $num_features;

    // The minimum interval length is 1.
    if ($int_length < 1) {
      $int_length = 1;
    }

    // Loop through every line in a file.
    //while($line = fgets($mat_fp) and (intval(ftell($mat_fp)) < $num_lines) {

    print "Loading complete: " . 0 . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";

    for ($j = 0; $j < $num_lines and $line = fgets($mat_fp); $j++) {

      if ($j % $load_limit == 0) {
        $set_fp = ftell($mat_fp);
        fclose($mat_fp);
        $time_end = microtime(TRUE);
        if ($j != 0) {
          print $load_limit / ($time_end - $time_start) . " lines per second\n";
        }
        $time_start = microtime(TRUE);
        $mat_fp = fopen($filepath, 'r');
        fseek($mat_fp, $set_fp);
      }

      if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
        $data_ln = 0;
      }
      if ($data_ln == 1 or !$re_start) {
        $linepart = preg_split('/\t/', $line, -1, PREG_SPLIT_NO_EMPTY);
        // Trim whitespace from columns.
        $new_linepart = [];
        foreach ($linepart as $col) {
          $new_linepart[] = trim($col);
        }
        // keep zero values, but get rid of possible null, false, and empty strings
        $linepart = array_filter($new_linepart, 'strlen');
        $num_col = count($linepart);
        $linepart = array_values($linepart);

        if ($mat_head == 0) {
          foreach ($linepart as $sample) {
            $quant_array[] = expression_create_biomaterial_structure($sample, $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $uniq_name);
          }

          // Make sure there not duplicate biomaterial names.
          if (count($bio_repeats = array_diff_assoc($linepart, array_unique($linepart))) > 0) {
            //if (count($linepart) != count(array_unique($linepart))) {
            print "ERROR: There are multiple instance of the following " . "biomaterial(s) in the expression file:\n";
            foreach ($bio_repeats as $repeat) {
              print "  " . $repeat . "\n";
            }
            print "Please make sure there is only one instance of each " . "biomaterial in the expression file\n";
            exit;
          }

          $num_biomaterials = count($linepart);
          $mat_head = 1;
          continue;
        }
        else {

          // Make sure that there are enough expression columns.
          if ($num_col != ($num_biomaterials + 1)) {
            print "ERROR: Expression data in the data section of the matrix format must have a\n" . "feature name followed by tab separated expression values.\n" . "The number of expression values for each feature must equal\n" . "the number of biomaterials in the matrix file header.\n" . "Example matrix format:\n" . "  <feature name>       <expression value>	<expression value> ...\n";
            if (count($linepart) > 0) {
              print "Loader failed on the following line:\n" . "  " . $line;
              print "Loader expected " . ($num_biomaterials + 1) . " columns " . "but only found " . $num_col . " column(s).\n";
            }
            else {
              print "Loader failed because of a blank line in the data section of the expression file.\n";
            }
            exit;
          }

          //exit;

          if ($cur_feature % $int_length == 0) {
            $percent = sprintf("%.2f", ($cur_feature / $num_features) * 100);
            print "Loading complete: " . $percent . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \n";
          }
          //  if ($j % $load_limit == 0) {
          //   print "Processing Feature $cur_feature of $num_features\n";
          // }
          $cur_feature++;
          for ($i = 1; $i < $num_col; $i++) {
            $feature_id = tripal_expression_find_feature_id($linepart[0], $organism_id, $feature_uniquenames);
            $element_id = tripal_expression_element($arraydesign_id, $feature_id);
            $elementresult_id = tripal_expression_elementresult($element_id, $quant_array[$i - 1], $linepart[$i]);
            // Create a record in the analysisfeature table. This will allow the analysis to appear in the
            // analysis list on the feature page.
            if ($i == 1) {
              $analysisfeature_id = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id', [
                ':analysis_id' => $analysis_id,
                ':feature_id' => $feature_id,
              ])->fetchField();
              if (!$analysisfeature_id) {
                $analysisfeature = chado_insert_record('analysisfeature', [
                  'analysis_id' => $analysis_id,
                  'feature_id' => $feature_id,
                ]);
                if ($analysisfeature == FALSE) {
                  print "ERROR: Unable to write to analysisfeature table\n";
                  exit;
                }
              }
              /*if($analysisfeature) {
                $analysisfeature = chado_update_record('analysisfeature',
                  array(':analysis_id' => $analysis_id, ':feature_id' => $feature_id),
                  $values);
                if($analysisfeature==FALSE) {
                  print "ERROR: Unable to update analysisfeature table\n";
                  exit;
                }
              } */
            }
          }
        }
      }
      if (preg_match('/' . $re_start . '/', $line) and $re_start) {
        $data_ln = 1;
      }
    }
    fclose($mat_fp);
    return;
  }

  /**
   *
   * Implements the function that will parse an expression file in the column
   * format.
   *
   * @param $organism_id
   *  The organism_id of the organism from which the biomaterial was gathered.
   *
   * @param $analysis_id
   *  The analysis_id of the analysis: expression from which the expression
   *   loader is being called.
   *
   * @param $contact_id
   *  The contact_id of the contact that provided the biomaterial. This
   *   paramter
   *   is optional.
   *
   * @param $arraydesign_id
   *  The arraydesign_id of the arraydesign associated with the experiment.
   *   This
   *   parameter is optional.
   *
   * @param $assay_id
   *  The assay_id of the assay associated with the experiment. This parameter
   *   is optional.
   *
   * @param $acquisition_id
   *  The acquisition_id of the acquisition associated with the experiment.
   *   This
   *   parameter is optional.
   *
   * @param $quantificatin_id
   *  The quantification_id of the quantification associated with the
   *   experiment. This parameter is optional.
   *
   * @param $filepath
   *  The file path to the expression file/s to be loaded.
   *
   * @param $fileext
   *  The file extension of the expression files to be loaded.
   *
   * @ingroup tripal_analysis_expression
   */

  function tripal_expression_load_col_file(
    $filepath,
    $fileext,
    $arraydesign_id,
    $organism_id,
    $analysis_id,
    $contact_id,
    $assay_id,
    $acquisition_id,
    $quantification_id,
    $re_start,
    $re_stop,
    $uniq_name,
    $feature_uniquenames
  ) {
    $inter = 0;
    $data_ln = 0;
    preg_match('/^.+\/(.+)\.' . $fileext . '$/', $filepath, $biomaterial);
    $quantification_id = expression_create_biomaterial_structure($biomaterial[1], $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $uniq_name);

    $features = [];

    $col_head = 0;
    $num_features = 0;

    $col_fp = fopen($filepath, 'r');
    // Ignore the feature footer.
    while ($line = fgets($col_fp)) {
      $num_lines++;
      if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
        $data_ln = 0;
      }
      // Grab data lines. Ingore the header.
      if ($data_ln == 1 or !$re_start) {
        if ($col_head == 0) {
          $num_features++;
          $col_head = 1;
          continue;
        }
        elseif ($col_head == 1) {
          $num_features++;
          preg_match("/^(.+?)\t/", $line, $match);
          $feature_array[] = trim($match[1]);
        }
      }
      // Only capture lines after the header.
      if (preg_match('/' . $re_start . '/', $line) and $re_start) {
        $data_ln = 1;
      }
    }

    fclose($col_fp);
    $data_ln = 0;

    // Make sure there not duplicate feature names.
    if (count($feature_repeats = array_diff_assoc($feature_array, array_unique($feature_array))) > 0) {
      //if (count($linepart) != count(array_unique($linepart))) {
      print "ERROR: There are multiple instance of the following " . "feature(s) in the expression file:\n";
      foreach ($feature_repeats as $repeat) {
        print "  " . $repeat . "\n";
      }
      print "Please make sure there is only one instance of each " . "feature in the expression file\n";
      exit;
    }

    // Get rid of the feature_array.
    unset($feature_array);

    // Calculate interval length.
    $int_length = .01 * $num_features;

    // The minimum interval length is 1.
    if ($int_length < 1) {
      $int_length = 1;
    }

    $f_id = 0;
    $e_id = 0;
    $er_id = 0;
    $af_id = 0;

    print "Loading complete: " . 0 . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";

    $col_fp = fopen($filepath, 'r');
    while ($line = fgets($col_fp)) {
      if (preg_match('/' . $re_stop . '/', $line) and $re_stop) {
        $data_ln = 0;
      }
      if ($data_ln == 1 or !$re_start) {
        //print "this is the line >>> $line <<<";
        $inter++;
        if ($inter % $int_length == 0) {
          $percent = sprintf("%.2f", ($inter / $num_features) * 100);
          print "Loading complete: " . $percent . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \r";
          ob_end_flush();
          //  print "Find feature_id: " . $f_id/($num_features*.01) . "\n";
          //  print "Create element_id: " . $e_id/($num_features*.01) . "\n";
          //  print "Create elementresult_id: " . $er_id/($num_features*.01) . "\n";
          //  print "Create analysisfeature_id: " . $af_id/($num_features*.01) . "\n\n";

          $f_id = 0;
          $e_id = 0;
          $er_id = 0;
          $af_id = 0;
        }
        $linepart = preg_split('/\t/', $line, -1, PREG_SPLIT_NO_EMPTY);

        // Trim whitespace from columns.
        $new_linepart = [];
        foreach ($linepart as $col) {
          $new_linepart[] = trim($col);
        }
        $linepart = array_filter($new_linepart, function ($var) {
          return $var != "";
        });
        $linepart = array_values($linepart);

        // Make sure that there are two columns.
        if (count($linepart) != 2) {
          print "ERROR: Expression data in the data section of the column format must have two columns separated by a tab.\n" . "Example column format:\n" . "  <feature name>	<expression value>\n";
          if (count($linepart) > 0) {
            print "Loader failed on the following line:\n" . "  " . $line . "\n";
          }
          else {
            print "Loader failed because of a blank line in the data section of the expression file.\n";
          }
          exit;
        }

        $num_col = count($linepart);

        // Extract the feature id.
        $start = microtime(TRUE);
        $feature_id = tripal_expression_find_feature_id($linepart[0], $organism_id, $feature_uniquenames);

        // Add to feature list;
        $features[] = $feature_id;

        //print "Find feature_id: ";
        $f_id += microtime(TRUE) - $start . "\n";

        $start = microtime(TRUE);
        $element_id = tripal_expression_element($arraydesign_id, $feature_id);
        //print "Create element_id: ";
        $e_id += microtime(TRUE) - $start . "\n";

        $start = microtime(TRUE);
        $elementresult_id = tripal_expression_elementresult($element_id, $quantification_id, $linepart[1]);
        //print "Create elementresult_id: ";
        $er_id += microtime(TRUE) - $start . "\n";
      }
      if (preg_match('/' . $re_start . '/', $line) and $re_start) {
        $data_ln = 1;
      }
    }
    fclose($col_fp);
    return $features;
  }

  /**
   *
   * Implements a function that find the find the feature_id from a feature
   * name.
   *
   * @param $feature
   *  The name of the feature to be found.
   *
   * @ingroup tripal_analysis_expression
   */

  function tripal_expression_find_feature_id(
    $feature,
    $organism_id,
    $feature_uniquenames
  ) {

    if ($feature_uniquenames == 'uniq') {
      $sql = "SELECT feature_id, organism_id FROM {feature} WHERE uniquename = :feature";
    }
    else {
      $sql = "SELECT feature_id, organism_id FROM {feature} WHERE name = :feature";
    }

    $feature_object = chado_query($sql, [':feature' => $feature])->fetchObject();

    if (!$feature_object) {
      print "ERROR: The feature, " . $feature . ", found in the expression file " . "was not found in the Chado database. Please ensure that the " . "feature has been loaded into the database and that the feature name is " . "both unique and correct.\n\n";
      exit;
    }

    if ($feature_object->organism_id != $organism_id) {
      $sql = "SELECT common_name, genus, species FROM {organism} WHERE organism_id = :organism_id";
      $analysis_org = chado_query($sql, [":organism_id" => $organism_id])->fetchObject();
      $feature_org = chado_query($sql, [":organism_id" => $feature_object->organism_id])->fetchObject();
      print "ERROR: Mismatch between the organism selected in the expression analysis " . "and the organism of a feature from the expression file.\n" . "  Analysis organism: " . $analysis_org->genus . " " . $analysis_org->species . " (" . $analysis_org->common_name . ")\n" . "  Feature organism: " . $feature_org->genus . " " . $feature_org->species . " (" . $feature_org->common_name . ")\n" . "  On feature: " . $feature . "\n" . "Please ensure that the organism selected for the expression " . "analysis and the organism for each feature in the expression " . "file are the same.\n";
      exit;
    }

    return $feature_object->feature_id;
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
    $element_id = chado_query('SELECT element_id FROM {element} WHERE arraydesign_id = :arraydesign_id AND feature_id = :feature_id', [
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
        print "ERROR: Unable to write to element table\n";
        exit;
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
   *  The quantification_id of the quantification associated with the
   *   experiment. This parameter is necessary for the creation of the
   *   elementresult record.
   *
   * @ingroup tripal_analysis_expression
   */

  function tripal_expression_elementresult(
    $element_id,
    $quantification_id,
    $signal
  ) {

    $values = [
      'element_id' => $element_id,
      'quantification_id' => $quantification_id,
      'signal' => $signal,
    ];

    $elementresult_id = chado_query('SELECT elementresult_id FROM {elementresult} WHERE element_id = :element_id AND quantification_id = :quantification_id', [
      ':element_id' => $element_id,
      ':quantification_id' => $quantification_id,
    ])->fetchField();
    if (!$elementresult_id) {
      $elementresult = chado_insert_record('elementresult', $values);
      if ($elementresult == FALSE) {
        print "ERROR: Unable to write to elementresult table\n";
        exit;
      }
    }
    if ($elementresult_id) {
      $elementresult = chado_update_record('elementresult', [
        'element_id' => $element_id,
        'quantification_id' => $quantification_id,
      ], $values);
      if ($elementresult == FALSE) {
        print "ERROR: Unable to update elementresult table\n";
        exit;
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
   *  The contact_id of the contact that provided the biomaterial. This
   *   paramter
   *   is optional. If the parameter is not provided, a generic contact record
   *   will be created in the contact table.
   *
   * @param $arraydesign_id
   *  The arraydesign_id of the arraydesign associated with the experiment.
   *
   * @param $assay_id
   *  The assay_id of the assay associated with the experiment. This parameter
   *   is optional. If the parameter is not provided, a generic assay record
   *   will be created in the assay table.
   *
   * @param $acquisition_id
   *  The acquisition_id of the acquisition associated with the experiment.
   *   This
   *   parameter is optional. If the parameter is not provided, a generic
   *   acquisition record will be created in the acquisition table.
   *
   * @param $quantificatin_id
   *  The quantification_id of the quantification associated with the
   *   experiment. This parameter is optional. If the parameter is not
   *   provided, a generic quantification record will be created in the
   *   quantification table.
   *
   * @ingroup tripal_analysis_expression
   */

  function expression_create_biomaterial_structure(
    $biomaterial,
    $organism_id,
    $analysis_id,
    $contact_id,
    $arraydesign_id,
    $assay_id,
    $acquisition_id,
    $quantification_id,
    $uniq_name
  ) {
    // Create the biomaterial
    $biomaterial_id = create_biomaterial($biomaterial, $analysis_id, $organism_id, $contact_id, $dbxref_id, '');

    $uniq_name = $biomaterial . $uniq_name;

    // Create the following records if not provided by the user.
    $contact_id = $contact_id ? $contact_id : create_generic_contact();
    $assay_id = $assay_id ? $assay_id : create_generic_assay($contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
    $acquisition_id = $acquistion_id ? $acquisition_id : create_generic_acquisition($assay_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
    $quantification_id = $quantification_id ? $quantification_id : create_generic_quantification($acquisition_id, $biomaterial_id, $organism_id, $analysis_id, $uniq_name);
    //$assay_id         = $assay_id          ? create_unique_assay($assay_id, $contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id)
    // : create_generic_assay($contact_id, $arraydesign_id, $biomaterial_id, $organism_id, $analysis_id);
    //$acquisition_id   = $acquisition_id    ? create_unique_acquisition($acquisition_id, $assay_id, $biomaterial_id, $organism_id, $analysis_id)
    //  : create_generic_acquisition($assay_id, $biomaterial_id, $organism_id, $analysis_id);
    // $quantification_id = $quantification_id ? create_unique_quantification($quantification_id, $acquisition_id, $biomaterial_id, $organism_id, $analysis_id)
    // : create_generic_quantification($acquisition_id, $biomaterial_id, $organism_id, $analysis_id);

    // Create a generic channel. A channel_id is necessary for the assay_biomaterial table.
    $channel_id = create_generic_channel();
    $assay_biomaterial_id = create_assay_biomaterial($assay_id, $biomaterial_id, $channel_id);

    return $quantification_id;
  }

  /**
   *
   * Implements a function to create a record in the assay_biomaterial table.
   * The assay_biomaterial table connects the assay and biomaterial table. As
   * well as an assay and biomaterial record, the assay_biomaterial table also
   * requires a channel record.
   *
   * @param $assay_id
   *  The assay_id of the assay associated with the experiment. This parameter
   *   is necessary for the creation of an assay_biomaterial record.
   *
   * @param $biomaterial_id
   *  The biomaterial_id of one of the biomaterials in the experiment. This
   *   parameter is necessary for the creation of the assay_biomaterial record.
   *
   * @param $channel_id
   *  The channel_id of the channel of the assay containing the biomaterial.
   *   This parameter is necessary for the creation of the assay_biomaterial
   *   record.
   *
   * @ingroup tripal_analysis_expression
   */

  function create_assay_biomaterial($assay_id, $biomaterial_id, $channel_id) {
    $assay_biomaterial_id = chado_query('select assay_biomaterial_id from {assay_biomaterial} where rank = :rank and assay_id = :assay_id and biomaterial_id = :biomaterial_id and channel_id = :channel_id limit 1', [
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
    $contact_id = chado_query('select contact_id from {contact} where name = :name limit 1', [':name' => 'Not provided'])->fetchField();
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
    $channel_id = chado_query('select channel_id from {channel} where name = :name limit 1', [':name' => 'Not provided'])->fetchField();
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
    $arraydesign_id = chado_query('select arraydesign_id from {arraydesign} where name = :name limit 1', [':name' => 'Not provided'])->fetchField();
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

  function create_unique_arraydesign(
    $arraydesign_id,
    $organism_id,
    $analysis_id
  ) {
    $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', [':analysis_id' => $analysis_id])->fetchField();

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

  function create_unique_assay(
    $assay_id,
    $contact_id,
    $arraydesign_id,
    $biomaterial_id,
    $organism_id,
    $analysis_id
  ) {
    $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', [':biomaterial_id' => $biomaterial_id])->fetchField();
    $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', [':analysis_id' => $analysis_id])->fetchField();

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

  function create_unique_acquisition(
    $acquisition_id,
    $assay_id,
    $biomaterial_id,
    $organism_id,
    $analysis_id
  ) {
    $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', [':biomaterial_id' => $biomaterial_id])->fetchField();
    $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', [':analysis_id' => $analysis_id])->fetchField();

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

  function create_unique_quantification(
    $quantification_id,
    $acquisition_id,
    $biomaterial_id,
    $organism_id,
    $analysis_id
  ) {
    $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', [':biomaterial_id' => $biomaterial_id])->fetchField();
    $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $organism_s = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', [':organism_id' => $organism_id])->fetchField();
    $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', [':analysis_id' => $analysis_id])->fetchField();

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
   * biomaterial_id, organism_id, and analysis_id are provided to the function
   * to create a unique assay name.
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
   *  The biomaterial_id of one of the biomaterials of the experiment. This
   *   will
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

  function create_generic_assay(
    $contact_id,
    $arraydesign_id,
    $biomaterial_id,
    $organism_id,
    $analysis_id,
    $uniq_name
  ) {
    /*$biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
    $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
    $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
    $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField(); */
    $assay_id = chado_query('select assay_id from {assay} where name = :name limit 1', [':name' => $uniq_name])->fetchField();
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

  function create_biomaterial(
    $biomaterial,
    $analysis_id,
    $organism_id,
    $biosourceprovider_id,
    $dbxref_id,
    $biomaterial_description
  ) {
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

  /**
   *
   * Implements a function to create a generic acquisition. Only a name and
   * assay_id, are required to create an assay record. A biomaterial_id,
   * organism_id, and analysis_id are provided to the function to create a
   * unique acquisition name.
   *
   * @param $assay
   *  The assay_id of the assay with which the acquisition is associated.
   *  This parameter is necessary for the creation of the acquisition record.
   *
   * @param $biomaterial_id
   *  The biomaterial_id of one of the biomaterials of the experiment. This
   *   will
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

  function create_generic_acquisition(
    $assay_id,
    $biomaterial_id,
    $organism_id,
    $analysis_id,
    $uniq_name
  ) {
    /* $biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
     $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
     $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
     $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField();*/
    $acquisition_id = chado_query('select acquisition_id from {acquisition} where name = :name limit 1', [':name' => $uniq_name])->fetchField();
    //$acquisition_id = chado_query('select acquisition_id from {acquisition} where name = :name limit 1',array(':name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis))->fetchField();
    if (!$acquisition_id) {
      $values = [
        'name' => $uniq_name,
        //biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
        //'name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
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
   *
   * Implements a function to create a generic quantification. Only a name,
   * acuqisition_id, and analysis_id are required to create an assay record. A
   * biomaterial_id, organism_id, and analysis_id are provided to the function
   * to create a unique quantification name.
   *
   * @param $acquisition
   *  The acquisition_id of the acquisition with which the quantification is
   *   associated. This parameter is necessary for the creation of the
   *   quantification record.
   *
   * @param $biomaterial_id
   *  The biomaterial_id of one of the biomaterials of the experiment. This
   *   will
   *   be used to create a unique name for the quantification.
   *
   * @param $organism_id
   *  The organism_id of the organism associated with the experiment. This will
   *   be used to create a unique name for the quantification.
   *
   * @param $analysis_id
   *  The analysis_id of the analysis associated with the experiment. This
   *   parameter is necessary for the creation of the quantification record.
   *   This record will also be used to create a unique name for the
   *   quantification.
   *
   * @ingroup tripal_analysis_expression
   */

  function create_generic_quantification(
    $acquisition_id,
    $biomaterial_id,
    $organism_id,
    $analysis_id,
    $uniq_name
  ) {
    /*$biomaterial = chado_query('SELECT name FROM {biomaterial} WHERE biomaterial_id = :biomaterial_id', array(':biomaterial_id' => $biomaterial_id))->fetchField();
    $organism_g = chado_query('SELECT genus FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
    $organism_s  = chado_query('SELECT species FROM {organism} WHERE organism_id = :organism_id', array(':organism_id' => $organism_id))->fetchField();
    $analysis = chado_query('SELECT name FROM {analysis} WHERE analysis_id = :analysis_id', array(':analysis_id' => $analysis_id))->fetchField(); */
    $quantification_id = chado_query('select quantification_id from {quantification} where name = :name limit 1', [':name' => $uniq_name])->fetchField();
    //$quantification_id = chado_query('select quantification_id from {quantification} where name = :name limit 1',array(':name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis))->fetchField();
    if (!$quantification_id) {
      $values = [
        'name' => $uniq_name,
        //'name' => $biomaterial . ' - ' . $organism_g . " " . $organism_s . ' - ' . $analysis,
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
}
