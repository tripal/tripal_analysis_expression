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
  public static $file_types = ['csv', 'tsv'];

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
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  public static $file_required = TRUE;

  /**
   * The publication ID for feature_cvterm.
   */
  private $pub_id;
  /**
   * The id of the cvterm to insert into feature_cvterm.
   */
  private $cvterm_id;
  /**
   * The id of the other cvterm.
   */
  private $cvalue_id;

  /**
   * The type id to be used when inserting the other cvterm as a prop.
   */
  private $cvalue_type_id;

  /**
   * The cvterm id for p-value.
   */
  private $pvalue_cvterm_id;

  /**
   * The cvterm id for analysis.
   */
  private $analysis_cvterm_id;

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
    // neither file_upload nor file_upload_existing default to null
    if (empty($values['file_local']) && empty($values['file_upload']) && empty($values['file_upload_existing'])) {
      return;
    }

    if (!empty($values['file_local'])) {
      if (!file_exists($values['file_local'])) {
        form_set_error('file_local', 'Local file path provided does not exist. Please ensure the file path is valid and try again.');
        return;
      }
      if (is_dir($values['file_local'])) {
        form_set_error('file_local', 'File provided is a directory. Please upload a .csv file.');
        return;
      }
    }

    $file_path = $this->findFile($values);
    if (!$file_path) {
      form_set_error('file_upload', 'Failed to get file path.');
      return;
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

    $file_info = pathinfo($file_path);
    $file_name = $file_info['filename'];
    $file_ext = $file_info['extension'];
    $cvterms = explode('_', $file_name);
    $cvterm_name = $cvterms[0];
    $cvalue_name = isset($cvterms[1]) ? $cvterms[1] : NULL;

    $this->pub_id = db_query('SELECT pub_id FROM chado.pub WHERE title IS NULL ORDER BY pub_id ASC limit 1')
      ->fetchField();
    $this->cvterm_id = $this->get_cvterm_id($cvterm_name);
    $this->cvalue_id = $this->get_cvterm_id($cvalue_name);
    $this->analysis_cvterm_id = $this->get_cvterm_id('analysis');

    // HARD CODED VALUES, TESTING PURPOSES ONLY
    $this->cvalue_type_id = 343;
    $this->pvalue_cvterm_id = 344;

    $this->parseFile($file_path, $file_ext, $analysis_id, $organism_id, $this->cvterm_id, $this->cvalue_id, $cvalue_name);
  }

  /**
   * Creates cvterms and cvtermprops for pvalues and features from the file.
   *
   * @param $file_path
   * @param $file_ext
   *  Either tsv or csv, determines delimiter for fgetcsv.
   * @param $organism_id
   * @param $analysis_id
   * @param $cvterm_id
   *  The cvterm from the first word of the filename.
   * @param $cvalue_id
   *  The cvterm from subsequent words of the filename.
   * @param $cvalue_name
   *  The string name of the cvalue_id.
   * @throws \Exception
   */
  private function parseFile($file_path, $file_ext, $analysis_id, $organism_id, $cvterm_id, $cvalue_id, $cvalue_name) {
    $file_size = filesize($file_path);
    $this->setTotalItems($file_size);
    $this->setItemsHandled(0);
    $bytes_read = 0;
    $delimiter = $file_ext == 'csv' ? ',' : "\t";

    $fp = fopen($file_path, 'r');

    while ($line = fgetcsv($fp, 0, $delimiter)) {
      $feature_name = $line[0];
      $p_adj = $line[1];

      $bytes_read += drupal_strlen($feature_name);
      $bytes_read += drupal_strlen($p_adj);
      $this->setItemsHandled($bytes_read);

      $feature_id = $this->get_feature_id($feature_name);
      $feature_cvterm_id = $this->insert_feature_cvterm($feature_id, $cvterm_id);

      // associated cvterm
      $this->insert_feature_cvtermprop($feature_cvterm_id, $this->cvalue_type_id, $cvalue_name, $cvalue_id);

      // pvalue
      $this->insert_feature_cvtermprop($feature_cvterm_id, $this->pvalue_cvterm_id, $p_adj);

      // analysis
      $this->insert_feature_cvtermprop($feature_cvterm_id, $this->analysis_cvterm_id, $analysis_id);
    }

    fclose($fp);
  }

  /**
   * Get the filepath of the pvalue file (local, upload, or upload
   * existing).
   *
   * Originally from tripal_biomaterial.
   *
   * @param $values
   *  The array of values from form_state.
   *
   * @return bool|string
   *  String containing the path to the file, or false on failure.
   */

  private function findFile($values) {

    if (!empty($values['file_local'])) {
      $file_path = trim($values['file_local']);
      return $file_path;

    } else if (!empty($values['file_upload'])) {
      $fid = trim($values['file_upload']);
      if (!$fid) {
        return false;
      }
      $file = file_load($fid);
      if (!$file) {
        return false;
      }
      $file_path = base_path() . drupal_realpath($file->uri);
      return $file_path;

    } else if (!empty($values['file_upload_existing'])) {

      $fid = $values['file_upload_existing'];
      if (!$fid) {
        return false;
      }
      $file = file_load($fid);
      if (!$file) {
        return false;
      }
      $file_path = base_path() . drupal_realpath($file->uri);
      return $file_path;
    }

    return false;
  }

  /**
   * Inserts a new entry into the feature_cvterm table and returns the id of the new record.
   *
   * @param $feature_id
   *  The feature with which to associate this cvterm.
   * @return \DatabaseStatementInterface|int
   *  The id of the inserted feature cvterm.
   * @throws \Exception
   */
  private function insert_feature_cvterm($feature_id) {
    $feature_cvterm_id = db_insert('chado.feature_cvterm')
      ->fields([
        'feature_id' => $feature_id,
        'cvterm_id' => $this->cvterm_id,
        'pub_id' => $this->pub_id,
      ])
      ->execute();

    return $feature_cvterm_id;
  }

  /**
   * Inserts a new entry into the feature_cvtermprop table and returns the id of the new record.
   *
   * @param $feature_cvterm_id
   *  The id of the feature cvterm with which to associate this cvtermprop.
   *  @see tripal_expression_pvalue_loader::insert_feature_cvterm()
   * @param $type_id
   *  Value to be inserted into the type_id column.
   * @param $value
   *  Value to be inserted into the value column.
   * @param null $cvalue_id
   *  Optional parameter indicating an associated cvterm.
   *
   * @return \DatabaseStatementInterface|int
   *  The id of the inserted feature cvtermprop.
   *
   * @throws \Exception
   */
  private function insert_feature_cvtermprop($feature_cvterm_id, $type_id, $value, $cvalue_id = NULL) {
    $fields = [];
    $fields['feature_cvterm_id'] = $feature_cvterm_id;
    $fields['type_id'] = $type_id;
    $fields['value'] = $value;
    if (isset($cvalue_id)) {
      $fields['cvalue_id'] = $cvalue_id;
    }

    $feature_cvtermprop_id = db_insert('chado.feature_cvtermprop')
      ->fields($fields)
      ->execute();

    return $feature_cvtermprop_id;
  }

  /**
   * Get the ID of the cvterm with a known name.
   *
   * @param $cvterm_name
   * @return int
   *  Integer ID of the cvterm.
   * @throws \Exception
   */
  private function get_cvterm_id($cvterm_name) {
    $cvterm_id = db_query('SELECT cv.cvterm_id FROM {chado.cvterm} cv WHERE cv.name = :cvterm_name',
      [':cvterm_name' => $cvterm_name])->fetchField();

    if (empty($cvterm_id)) {
      throw new Exception(t('The cvterm, !cvterm, does not exist in the database.',
        ['!cvterm' => $cvterm_name]));
    }

    return $cvterm_id;
  }

  /**
   * Get the ID of the cvterm with a known name.
   *
   * @param $feature_name
   * @return mixed
   *  Integer ID of the cvterm.
   * @throws \Exception
   */
  private function get_feature_id($feature_name) {
    $feature_id = db_query('SELECT f.feature_id FROM {chado.feature} f WHERE uniquename = :feature_name',
      [':feature_name' => $feature_name])->fetchField();

    if (empty($feature_id)) {
      throw new Exception(t('The feature, !feature, does not exist in the database.',
        ['!feature' => $feature_name]));
    }

    return $feature_id;
  }
}