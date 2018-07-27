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
  public static $machine_name = 'Chado_Expression_Data_Loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Import expression data.  Requires an analaysis and biomaterial.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['csv', 'tsv', 'txt'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Expression data can be loaded from two format types. Select the column format for files that have two columns - transcript id and expression value. Select the matrix format for files that specify biosample by column and transcript by row. If "Column Format" is selected, the name of the column file will be taken as the biomaterial name. It is recommended to avoid the use of white space in column file names and in biomaterial names.<br>  Please verify the column or file names match the intended biomaterial name in the database.';

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

    //protocols, assays, etc go here

    $form['experimental_design'] = [
      '#title' => t('Experimental Design'),
      '#type' => 'fieldset',
      '#description' => t('Describe the parameters from which the expression data was generated.'),
      '#attributes' => ['id' => 'expression-extra-settings'],
      '#weight' => 1,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
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

    //TODO: WE DONT USE THIS (BUT WE SHOULD)

    //    $lt_message = tripal_set_message(t('If the biomaterial provider is not present in the dropdown list, ') . l(t('add a new contact.'), 'bio_data/add') . t(' A biomaterial provider is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

//    $form['experimental_design']['biosourceprovider_id'] = [
//      '#title' => t('Biomaterial Provider'),
//      '#type' => 'select',
//      '#empty_value' => '',
//      '#description' => t('The contact who provided the biomaterial. If the biomaterial is already') . (' present in the database, the biomaterial provider selected here will') . (' overwrite the biomaterial provider set for the biomaterial in the database.'),
//      '#options' => $form['choose_contact']['#value'],
//      '#suffix' => $lt_message,
//    ];

    $lt_message = tripal_set_message(t('If the array design does not appear in this list, ') . l(t('add a new arraydesign.'), 'bio_data/add') . t(' If an array design is not selected, then a generic array design will be created for the analysis.'), TRIPAL_INFO, ['return_html' => TRUE]);

    $arraydesigns = tripal_get_arraydesign_select_options(FALSE);

    $form['experimental_design']['arraydesign_id'] = [
      '#title' => t('Array Design'),
      '#type' => 'select',
      '#description' => t('The array design associated with this analysis. This is not required if the experimental') . t(' data was gathered from next generation sequencing methods. If an array design is not') . t(' selected, a generic array design will be used as a place filler.'),
      '#options' => $arraydesigns,
      '#suffix' => $lt_message,
    ];

    //TODO: WE DONT USE THIS (BUT WE SHOULD)

    //
//    $form['experimental_design']['acquisition_details'] = [
//      '#title' => t('Acquisition Details'),
//      '#type' => 'fieldset',
//      '#description' => t('This represents the quantification technique. In the case of a microarray, it is scanning, in the case of a sequencer, it is sequencing. The output of this process is a digitial image of an array for a microarray or a set of digital images or nucleotide base calls for a sequencer.'),
//      '#collapsible' => TRUE,
//      '#collapsed' => FALSE,
//      '#weight' => 1,
//    ];

//    $form['experimental_design']['acquisition_details']['acquisitiondate'] = [
//      '#type' => 'date',
//      '#title' => t('Date Acquisition Run'),
//      '#description' => t('If no date is provided, then the current date will be used.'),
//      '#required' => TRUE,
//      '#default_value' => [
//        'year' => $year,
//        'month' => $month,
//        'day' => $day,
//      ],
//    ];

    //TODO: WE DONT USE THIS (BUT WE SHOULD)

    //    $form['experimental_design']['acquisition_details']['acquisition_uri'] = [
//      '#type' => 'textfield',
//      '#title' => t('Acquisition URI'),
//      '#description' => t('URI location that describes the acquisition.'),
//    ];

    $lt_message = tripal_set_message(t('If the acquisition protocol is not present in the dropdown list, ') . l(t('add a new protocol.'), 'bio_data/add') . t(' An acquisition protocol is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

    $acquisitions = tripal_get_protocol_select_options(FALSE, 'Acquisition Protocol');

    //TODO: WE DONT USE THIS (BUT WE SHOULD)

    //
//    $form['experimental_design']['acquisition_details']['acquisition_protocol_id'] = [
//      '#title' => t('Acquisition Protocol'),
//      '#type' => 'select',
//      '#description' => t('The protocol associated with this acquisition.'),
//      '#options' => $acquisitions,
//      '#suffix' => $lt_message,
//    ];

    $form['experimental_design']['quantification_details'] = [
      '#title' => t('Quantification Details'),
      '#type' => 'fieldset',
      '#description' => t(' Quantification is the transformation of an image or set of sequences to numeric expression data. This typically involves statistical procedures.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#weight' => 1,
    ];
    $form['experimental_design']['quantification_details']['quantificationunits'] = [
      '#type' => 'textfield',
      '#title' => t('Units'),
      '#description' => t('The units associated with the loaded values (ie, FPKM, RPKM, raw counts).'),
    ];
    //TODO: WE DONT USE THIS (BUT WE SHOULD)

//    $form['experimental_design']['quantification_details']['quantificationdate'] = [
//      '#type' => 'date',
//      '#title' => t('Date Quantification Run'),
//      '#description' => t('If no date is provided, then the current date will be used.'),
//      '#required' => TRUE,
//      '#default_value' => [
//        'year' => $year,
//        'month' => $month,
//        'day' => $day,
//      ],
//    ];


    //TODO: WE DONT USE THIS (BUT WE SHOULD)

    //    $form['experimental_design']['quantification_details']['quantification_uri'] = [
//      '#type' => 'textfield',
//      '#title' => t('Quantification URI'),
//      '#description' => t('URI location that describes the quantification.'),
//    ];
//
//    $lt_message = tripal_set_message(t('If the quantification operator is not present in the dropdown list, ') . l(t('add a new contact.'), 'bio_data/add') . t('A quantication operator is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);

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

    //TODO: WE DONT USE THIS (BUT WE SHOULD)
    //
//    $form['experimental_design']['quantification_details']['quantification_operator_id'] = [
//      '#title' => t('Quantification Operator'),
//      '#type' => 'select',
//      '#empty_value' => '',
//      '#description' => t('The quantification operator. If the operator does not appear in this list, ') . l(t('add a new contact.'), 'bio_data/add') . t(' An operator is not required.'),
//      '#options' => $form['choose_contact']['#value'],
//      '#suffix' => $lt_message,
//    ];


    //TODO: WE DONT USE THIS (BUT WE SHOULD)
//    $lt_message = tripal_set_message(t('If the quantification protocol is not present in the dropdown list, ') . l(t('add a new protocol.'), 'bio_data/add') . t(' A quantification protocol is not required.'), TRIPAL_INFO, ['return_html' => TRUE]);
//
//    $quantifications = tripal_get_protocol_select_options(FALSE, 'Quantification Protocol');

//    $form['experimental_design']['quantification_details']['quantification_protocol_id'] = [
//      '#title' => t('Quantification Protocol'),
//      '#type' => 'select',
//      '#description' => t('The protocol associated with this quantification.'),
//      '#options' => $quantifications,
//      '#suffix' => $lt_message,
//    ];

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

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */

  public function formValidate($form, &$form_state) {

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
    $fileext = $arguments['fileext'];
    $filetype = $arguments['filetype'];
    $re_start = $arguments['re_start'];
    $re_stop = $arguments['re_stop'];
    $organism_id = $arguments['organism_id'];
    if (isset($arguments['analysis_id'])) {
      $analysis_id = $arguments['analysis_id'];
    }
    $feature_uniquenames = $arguments['feature_uniquenames'];
    $arraydesign_id = $arguments['arraydesign_id'];

    $quantificationunits = $arguments['quantificationunits'];

    //we dont actually use these variables so lets not collect the data.
    //but, we do pass it around a million ways so lets declare the variables
    $assaydate = null;//$arguments['assaydate'];
    $contact_id = null;//$arguments['contact_id'];
    $acquisitiondate = null;// $acquisitiondate = $arguments['acquisitiondate'];
    $create_biosamples = null;// $create_biosamples = $arguments['create_biosamples'];
    $quantificationdate = null;// $quantificationdate = $arguments['quantificationdate'];
    //



    tripal_set_message(t("Job Submitted!  If new Biosamples will be created in this job, remember to") . l('publish them after the job runs.',
        'admin/content/bio_data/publish/'), TRIPAL_INFO);


    $this->tripal_expression_loader($organism_id, $analysis_id, $contact_id, $arraydesign_id, $assaydate, $acquisitiondate, $quantificationdate, $quantificationunits, $filepath, $fileext, $filetype, $re_start, $re_stop, $feature_uniquenames, $create_biosamples);
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

  protected function tripal_expression_loader(
    $organism_id,
    $analysis_id,
    $contact_id,
    $arraydesign_id,
    $assaydate,
    $acquisitiondate,
    $quantificationdate,
    $quantificationunits,
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

    $uniq_name = $this->get_unique_name($organism_id, $analysis_id);

    $transaction = db_transaction();

    try {
      // Create the array design.
      $arraydesign_id = $arraydesign_id ? $arraydesign_id : create_generic_arraydesign(create_generic_contact());
      $re_ext = '/^.+\.(' . $fileext . ')/';

      // A matrix file.
      if ($filetype == 'mat' and is_file($filepath) and preg_match($re_ext, $filepath)) {

        $this->tripal_expression_load_mat_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $quantificationunits, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
      }

      // The column format.
      elseif ($filetype == 'col' and is_file($filepath) and preg_match($re_ext, $filepath)) {
        $features = $this->tripal_expression_load_col_file($filepath, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $quantificationunits, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
      }
      else { // Multiple files in the column format.
        $dir_handle = @opendir($filepath) or die("Unable to open $filepath");
        $total_files = count(glob($filepath . '/*.' . $fileext));
        $num_file = 0;

        // Load all files with of type $fileext in the directory.
        while ($file = readdir($dir_handle)) {
          if (preg_match('/(.+)\.' . $fileext . '$/', $file)) {
            print "File " . ($num_file + 1) . " of $total_files: $file                                       \n";
            $new_features = $this->tripal_expression_load_col_file($filepath . '/' . $file, $fileext, $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id, $acquisition_id, $quantification_id, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
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
            return;
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
    $quantificationunits,
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

    if ($num_lines < 2){
      print "Error reading file.\n Please ensure that your file is readable and try again.";
      return;
    }

    // Make sure there not duplicate feature names.
    if (count($feature_repeats = array_diff_assoc($feature_array, array_unique($feature_array))) > 0) {
      //if (count($linepart) != count(array_unique($linepart))) {
      print "ERROR: There are multiple instance of the following feature(s) in the expression file:\n";
      foreach ($feature_repeats as $repeat) {
        print "  " . $repeat . "\n";
      }
      print "Please make sure there is only one instance of each feature in the expression file\n";
      return;
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
            $quant_array[] = expression_create_biomaterial_structure($sample, $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $quantificationunits, $uniq_name);
          }

          // Make sure there not duplicate biomaterial names.
          if (count($bio_repeats = array_diff_assoc($linepart, array_unique($linepart))) > 0) {
            //if (count($linepart) != count(array_unique($linepart))) {
            print "ERROR: There are multiple instance of the following " . "biomaterial(s) in the expression file:\n";
            foreach ($bio_repeats as $repeat) {
              print "  " . $repeat . "\n";
            }
            print "Please make sure there is only one instance of each " . "biomaterial in the expression file\n";
            return;
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
            return;
          }

          //return;

          if ($cur_feature % $int_length == 0) {
            $percent = sprintf("%.2f", ($cur_feature / $num_features) * 100);
            print "Loading complete: " . $percent . "%. Memory: " . number_format(memory_get_usage()) . " bytes. \n";
          }
          //  if ($j % $load_limit == 0) {
          //   print "Processing Feature $cur_feature of $num_features\n";
          // }
          $cur_feature++;
          for ($i = 1; $i < $num_col; $i++) {
            $feature_id = $this->tripal_expression_find_feature_id($linepart[0], $organism_id, $feature_uniquenames);
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
                  return;
                }
              }
              /*if($analysisfeature) {
                $analysisfeature = chado_update_record('analysisfeature',
                  array(':analysis_id' => $analysis_id, ':feature_id' => $feature_id),
                  $values);
                if($analysisfeature==FALSE) {
                  print "ERROR: Unable to update analysisfeature table\n";
                  return;
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
    $quantificationunits,
    $re_start,
    $re_stop,
    $uniq_name,
    $feature_uniquenames
  ) {
    $inter = 0;
    $data_ln = 0;
    preg_match('/^.+\/(.+)\.' . $fileext . '$/', $filepath, $biomaterial);
    $quantification_id = expression_create_biomaterial_structure($biomaterial[1], $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id, $acquisition_id, $quantification_id, $quantificationunits, $uniq_name);

    $features = [];

    $col_head = 0;
    $num_features = 0;
    $num_lines = 0;

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
      return;
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
          return;
        }

        $num_col = count($linepart);

        // Extract the feature id.
        $start = microtime(TRUE);
        $feature_id = $this->tripal_expression_find_feature_id($linepart[0], $organism_id, $feature_uniquenames);

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

  protected function tripal_expression_find_feature_id(
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
      return;
    }

    if ($feature_object->organism_id != $organism_id) {
      $sql = "SELECT common_name, genus, species FROM {organism} WHERE organism_id = :organism_id";
      $analysis_org = chado_query($sql, [":organism_id" => $organism_id])->fetchObject();
      $feature_org = chado_query($sql, [":organism_id" => $feature_object->organism_id])->fetchObject();
      print "ERROR: Mismatch between the organism selected in the expression analysis " . "and the organism of a feature from the expression file.\n" . "  Analysis organism: " . $analysis_org->genus . " " . $analysis_org->species . " (" . $analysis_org->common_name . ")\n" . "  Feature organism: " . $feature_org->genus . " " . $feature_org->species . " (" . $feature_org->common_name . ")\n" . "  On feature: " . $feature . "\n" . "Please ensure that the organism selected for the expression " . "analysis and the organism for each feature in the expression " . "file are the same.\n";
      return;
    }

    return $feature_object->feature_id;
  }
}
