<?php

/**
 * @see TripalImporter
 */
class tripal_expression_pvalue_loader extends TripalImporter {

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   */
  public static $name = 'Tripal Expression Pvalue Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   */
  public static $machine_name = 'tripal_expression_pvalue_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   */
  public static $description = 'Import pvalues for features.';

  /**
   * An array containing the extensions of allowed file types.
   */
  public static $file_types = ['csv'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   */
  public static $upload_description = 'Files containing pvalue must be uploaded as a two column tsv or csv file, where the first column consists of the names of the features and the second column consists of the adjusted p-values.';

  /**
   * Indicates the methods that the file uploader will support.
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => FALSE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  public static $file_required = TRUE;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#required' => TRUE,
      '#description' => t('The organism associated with the features.'),
      '#options' => chado_get_organism_select_options(FALSE),
    ];

    return $form;
  }

  /**
   * @see TripalImporter::formValidate()
   */
  public function formValidate($form, &$form_state) {

    parent::formValidate($form, $form_state);

    $values = $form_state['values'];
    if (!isset($values['file_local']) && !isset($values['file_upload']) && !isset($values['file_upload_existing'])) {
      return;
    }

    if (isset($values['file_local'])) {
      if (!file_exists($values['file_local'])) {
        form_set_error('file_local', 'Local file path provided does not exist. Please ensure the file path is valid and try again.');
        return;
      }
      if (is_dir($values['file_local'])) {
        form_set_error('file_local', 'File provided is a directory. Please upload a .csv file.');
        return;
      }
    }

    $file_path = '';

    if (isset($values['file_local']) || isset($values['file_upload']) || isset($values['file_upload_existing'])) {
      $file_path = $this->findFile($values);
      if (!$file_path) {
        form_set_error('file_upload', 'Failed to get file path.');
        return;
      }
    }

    $fp = fopen($file_path, 'r');

    if (!$fp) {
      form_set_error('file_upload', 'Failed to open file. Please try again.');
      return;
    }

    if (count(fgetcsv($fp, 0, ',')) != 2) {
      form_set_error('file_upload', 'File provided contains incorrect number of columns. This importer requires that the uploaded file contain exactly two columns containing the feature name and the pvalue.');
      return;
    }

    fclose($fp);
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $run_args  = $this->arguments['run_args'];
    $file_path = $this->arguments['files']['file_path'];
    if (array_key_exists('file_path', $this->arguments['files'][0])) {
      $file_path = $this->arguments['files'][0]['file_path'];
    }

    $analysis_id = $run_args['analysis_id'];
    $organism_id = $run_args['organism_id'];

    var_dump($this->arguments['files']);

    // $this->parseFile($file_path, $analysis_id, $organism_id);
  }

  /**
   * Creates cvterms and cvtermprops for pvalue from the file.
   *
   * @param $file_path
   * @param $analysis_id
   * @param $organism_id
   */
  private function parseFile($file_path, $analysis_id, $organism_id) {

    $file_size = filesize($file_path);
    $this->setTotalItems($file_size);
    $this->setItemsHandled(0);
    $bytes_read = 0;

    $fp = fopen($file_path, 'r');

    while ($line = fgetcsv($fp, 0, ',')) {
      $bytes_read += drupal_strlen($line);
      $this->setItemsHandled($bytes_read);

      $feature_name = $line[0];
      $p_adj = $line[1];
    }

    fclose($fp);
  }

  /**
   * Get the filepath of the pvalue file (local, upload, or upload
   * existing).
   *
   * Originally from tripal_biomaterial
   *
   * @param $values
   *  The array of values from form_state.
   *
   * @return null|string
   *  String containing the path to the file.
   */
  private function findFile($values) {

    $file_path = NULL;

    if (isset($values['file_local'])) {
      $file_path = trim($values['file_local']);
    }

    // value defaults to 0, isset will return true, so we use not empty instead
    else if (!empty($values['file_upload'])) {
      $fid = trim($values['file_upload'][0]);
      if (!$fid) {
        return false;
      }
      $file = file_load($fid);
      if (!$file) {
        return false;
      }

      $file_path = base_path() . drupal_realpath($file->uri);
    }

    else if (!empty($values['file_upload_existing'])) {
      $fid = $values['file_upload_existing'];
      if (!$fid) {
        return false;
      }
      $file = file_load($fid);
      if (!$file) {
        return false;
      }
      $file_path = base_path() . drupal_realpath($file->uri);
    }

    return $file_path;
  }
}