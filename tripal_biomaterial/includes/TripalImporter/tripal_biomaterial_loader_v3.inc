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
          ':input[name="file_type"]' => ['value' => 'flat_file'],
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
    dpm($form);
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    // Make sure an organism is selected.
    $organism_id = $form_state['values']['organism_id'];
    $file_type = $form_state['values']['file_type'];
    if (!$organism_id) {
      form_set_error('organism_id', 'Please select the organism from which the biomaterials were collected.');
    }

    //Make sure that a file has been uploaded OR a path has been set.

    //
    //if ($file_type == 'xml') {
    //  $path = $form_state['values']['xml_file']['xml_path'];
    //  if ($path == '') {
    //    form_set_error('xml_file][xml_path', 'Please enter the path to a file.');
    //  } //** abs or relative?
    //  else {
    //    if (! is_readable($path)) {
    //      form_set_error('xml_file][xml_path', 'File is not present or is unreadable.');
    //    }
    //  }
    //} else {
    //  $path = $form_state['values']['flat_file']['flat_path'];
    //  if ($path == '') {
    //    form_set_error('xml_file][xml_path', 'Please enter the path to a file.');
    //  } //** abs or relative?
    //  else {
    //    if (! is_readable($path)) {
    //      form_set_error('flat_file][flat_path', 'File is not present or is unreadable.');
    //    }
    //  }
    //}

    //// Make sure the flat file type is selected when loading a flat file.
    //$flat_file_type = $form_state['values']['flat_file']['flat_file_type'];
    //if ($file_type == 'flat' and ! $flat_file_type) {
    //  form_set_error('flat_file_type', 'Please select a flat file format.');
    //}
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $file_path = $this->arguments['file']['file_path'];
    dpm($this->arguments);
  }
}