<?php

/**
 * @see TripalImporter
 */
class tripal_expression_pvalue_loader extends TripalImporter{

  /**
   * The name of this loader.  This name will be presented to the site
   * user.
   *
   * @var string
   */
  public static $name = 'Tripal Expression Pvalue Loader';

  /**
   * The machine name for this loader. This name will be used to construct
   * the URL for the loader.
   *
   * @var string
   */
  public static $machine_name = 'tripal_expression_pvalue_loader';

  /**
   * A brief description for this loader.  This description will be
   * presented to the site user.
   *
   * @var string
   */
  public static $description = 'Import pvalues for features.';

  /**
   * An array containing the extensions of allowed file types.
   *
   * @var array
   */
  public static $file_types = ['csv', 'tsv'];

  /**
   * Provides information to the user about the file upload.  Typically this
   * may include a description of the file types allowed.
   *
   * @var string
   */
  public static $upload_description = 'Files containing pvalue must be uploaded as a two column tsv or csv file, where the first column consists of the names of the features and the second column consists of the adjusted p-values.';

  /**
   * Indicates the methods that the file uploader will support.
   *
   * @var array
   */
  public static $methods = [
    // Allow the user to upload a file to the server.
    'file_upload' => TRUE,
    // Allow the user to provide the path on the Tripal server for the file.
    'file_local' => TRUE,
    // Allow the user to provide a remote URL for the file.
    'file_remote' => FALSE,
  ];

  /**
   * @var bool
   */
  public static $file_required = TRUE;

  /**
   * The publication ID for feature_cvterm.
   *
   * @var int
   */
  private $pub_id;

  /**
   * The id of the cvterm to insert into feature_cvterm.
   *
   * @var int
   */
  private $cvterm_id;

  /**
   * The id of the other cvterm.
   *
   * @var int
   */
  private $evidence_code;

  /**
   * String name of the cvterm associated with the keyword.
   *
   * @var string
   */
  private $evidence_code_id;

  /**
   * The type id to be used when inserting the other cvterm as a prop.
   *
   * @var int
   */
  private $evidence_code_cvterm_id;

  /**
   * The cvterm id for p-value.
   *
   * @var int
   */
  private $pvalue_cvterm_id;

  /**
   * The cvterm id for analysis.
   *
   * @var int
   */
  private $analysis_cvterm_id;

  /**
   * ID for analysis.
   *
   * @var int
   */
  private $analysis_id;

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    $form['evidence_code'] = [
      '#title' => t('Evidence Code'),
      '#type' => 'select',
      '#description' => t('The evidence code that relates the cvterms.'),
      '#options' => drupal_map_assoc([
        'up-regulated',
        'down-regulated',
        'related to',
      ]),
      '#required' => TRUE,
    ];

    $form['keyword'] = [
      '#type' => 'textfield',
      '#title' => t('Experimental Factor'),
      '#required' => TRUE,
      '#description' => t('What is this gene responding to (e.g. heat, drought, ozone)? If the controlled vocabulary term does not show up, please ' . l('create it first.',
          'admin/tripal/loaders/chado_vocabs/chado_cv/cvterm/add')),
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/cvterm/0',
    ];

    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#required' => TRUE,
      '#description' => t('The organism for the features.'),
      '#options' => chado_get_organism_select_options(FALSE),
    ];

    $so_id = chado_query('SELECT cv_id FROM {cv} WHERE name=:cv_name',
      [':cv_name' => 'sequence'])->fetchField();

    $form['so_term'] = [
      '#type' => 'textfield',
      '#title' => t('Sequence Type'),
      '#required' => TRUE,
      '#description' => t('Please enter the Sequence Ontology (SO) term name that describes the features (e.g. gene, mRNA, polypeptide, etc...)'),
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/cvterm/' . $so_id,
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
        form_set_error('file_local',
          'Local file path provided does not exist. Please ensure the file path is valid and try again.');

        return;
      }
      if (is_dir($values['file_local'])) {
        form_set_error('file_local',
          'File provided is a directory. Please upload a .csv file.');

        return;
      }
    }

    if (!chado_get_cvterm(['name' => $values['keyword']])) {
      form_set_error('keyword', 'Cvterm provided does not exist.');

      return;
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
      form_set_error('file_upload',
        'File provided contains incorrect number of columns. This importer requires that the uploaded file contain exactly two columns containing the feature name and the pvalue.');

      return;
    }

    fclose($fp);
  }

  /**
   * @see TripalImporter::run()
   */
  public function run() {
    $run_args = $this->arguments['run_args'];
    $file_path = $this->arguments['files']['file_path'];
    if (array_key_exists('file_path', $this->arguments['files'][0])) {
      $file_path = $this->arguments['files'][0]['file_path'];
    }

    $file_info = pathinfo($file_path);
    $file_ext = $file_info['extension'];

    $cvterm_name = $run_args['keyword'];
    $this->cvterm_id = chado_get_cvterm(['name' => $cvterm_name])->cvterm_id;

    $this->evidence_code = $run_args['evidence_code'];
    $this->evidence_code_id = chado_get_cvterm(['name' => $this->evidence_code])->cvterm_id;
    $this->evidence_code_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'local'],
      'name' => 'evidence code',
    ])->cvterm_id;

    $this->analysis_cvterm_id = chado_get_cvterm(['name' => 'analysis'])->cvterm_id;
    $this->analysis_id = $run_args['analysis_id'];

    $organism_id = $run_args['organism_id'];
    $so_term = $run_args['so_term'];

    $this->pvalue_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'OBI'],
      'name' => 'p-value',
    ])->cvterm_id;

    $this->pub_id = chado_query('SELECT pub_id FROM {pub} WHERE title IS NULL ORDER BY pub_id ASC limit 1')->fetchField();

    $this->parseFile($file_path, $file_ext, $organism_id, $so_term);
  }

  /**
   * Creates cvterms and cvtermprops for pvalues and features from the file.
   *
   * @param string $file_path
   * @param string $file_ext
   *  Either tsv or csv, determines delimiter for fgetcsv.
   * @param $cvterm_id
   *  The cvterm from the first word of the filename.
   *
   * @throws \Exception
   */
  private function parseFile($file_path, $file_ext, $organism_id, $so_term) {
    $file_size = filesize($file_path);
    $this->setTotalItems($file_size);
    $this->setItemsHandled(0);
    $bytes_read = 0;
    $delimiter = $file_ext == 'csv' ? ',' : "\t";

    $fp = fopen($file_path, 'r');

    while ($line = fgetcsv($fp, 0, $delimiter)) {
      $feature_name = $line[0];
      $pvalue = $line[1] ? floatval($line[1]) : NULL;

      $bytes_read += drupal_strlen($feature_name);
      $bytes_read += drupal_strlen($pvalue);
      $this->setItemsHandled($bytes_read);

      $type_id = chado_get_cvterm(['name' => $so_term])->cvterm_id;

      $feature_id = $this->get_feature_id($feature_name, $organism_id,
        $type_id);

      $data = $this->get_existing_data($feature_id);

      is_object($data) ? $this->updateData($data->feature_cvterm_id,
        $pvalue) : $this->insertData($feature_id, $pvalue);
    }

    fclose($fp);
  }

  /**
   * Get the filepath of the pvalue file (local, upload, or upload
   * existing).
   *
   * Originally from tripal_biomaterial.
   *
   * @param array $values
   *  The array of values from form_state.
   *
   * @return bool|string
   *  String containing the path to the file, or false on failure.
   */

  private function findFile($values) {
    if (!empty($values['file_local'])) {
      $file_path = trim($values['file_local']);

      return $file_path;
    }
    else {
      if (!empty($values['file_upload'])) {
        $fid = trim($values['file_upload']);
        if (!$fid) {
          return FALSE;
        }
        $file = file_load($fid);
        if (!$file) {
          return FALSE;
        }
        $file_path = base_path() . drupal_realpath($file->uri);

        return $file_path;
      }
      else {
        if (!empty($values['file_upload_existing'])) {

          $fid = $values['file_upload_existing'];
          if (!$fid) {
            return FALSE;
          }
          $file = file_load($fid);
          if (!$file) {
            return FALSE;
          }
          $file_path = base_path() . drupal_realpath($file->uri);

          return $file_path;
        }
      }
    }

    return FALSE;
  }

  /**
   * Gets any existing p-value data that exists for the given feature.
   *
   * @param int $feature_id
   *  The id of the feature for which you want to find data.
   *
   * @return object|NULL
   * @throws \Exception
   */
  public function get_existing_data($feature_id) {
    $sql = 'SELECT feature_cvterm_id FROM {feature_cvterm} WHERE feature_id=:feature_id AND cvterm_id=:cvterm_id';

    $feature_cvterms = chado_query($sql, [
      ':feature_id' => $feature_id,
      ':cvterm_id' => $this->cvterm_id,
    ])->fetchAll();

    if (empty($feature_cvterms)) {
      return FALSE;
    }

    $analysis_cvterm_id = chado_get_cvterm(['name' => 'analysis'])->cvterm_id;

    foreach ($feature_cvterms as $feature_cvterm) {
      $feature_cvterm_id = $feature_cvterm->feature_cvterm_id;

      $props = chado_query('
        SELECT p1.feature_cvterm_id,
               p1.value AS evidence_code,
               p2.value AS p_value,
               p3.value AS analysis_id
        FROM {feature_cvtermprop} p1    
        JOIN {feature_cvtermprop} p2
        ON p2.feature_cvterm_id=p1.feature_cvterm_id
        AND p2.type_id=:pvalue_type_id
        JOIN {feature_cvtermprop} p3
        ON p3.feature_cvterm_id=p1.feature_cvterm_id
        AND p3.type_id=:analysis_cvterm_id
        AND p3.value=:analysis_id
        WHERE p1.feature_cvterm_id=:feature_cvterm_id
        AND p1.type_id=:evidence_code_cvterm_id
        AND p1.value=:evidence_code', [
        ':feature_cvterm_id' => $feature_cvterm_id,
        ':evidence_code_cvterm_id' => $this->evidence_code_cvterm_id,
        ':evidence_code' => $this->evidence_code,
        ':pvalue_type_id' => $this->pvalue_cvterm_id,
        ':analysis_cvterm_id' => $analysis_cvterm_id,
        ':analysis_id' => $this->analysis_id,
      ])->fetchObject();

      if (is_object($props)) {
        return $props;
      }
    }

    return NULL;
  }

  /**
   * Updates existing data.
   *
   * @param int $feature_cvterm_id
   * @param int $pvalue
   */
  private function updateData($feature_cvterm_id, $pvalue) {
    db_update('chado.feature_cvtermprop')
      ->fields([
        'value' => $pvalue,
      ])
      ->condition('feature_cvterm_id', $feature_cvterm_id, '=')
      ->condition('type_id', $this->pvalue_cvterm_id, '=')
      ->execute();
  }

  /**
   * Inserts new data.
   *
   * @param int $feature_id
   * @param int $pvalue
   *
   * @throws \Exception
   */
  private function insertData($feature_id, $pvalue) {
    $highest_rank = chado_query('SELECT rank FROM {feature_cvterm} WHERE feature_id=:feature_id ORDER BY rank DESC LIMIT 1',
      [':feature_id' => $feature_id])->fetchField();

    $rank = $highest_rank === FALSE ? 0 : $highest_rank + 1;

    $feature_cvterm_id = $this->insert_feature_cvterm($feature_id, $rank);

    // evidence code
    $this->insert_feature_cvtermprop($feature_cvterm_id,
      $this->evidence_code_cvterm_id, $this->evidence_code,
      $this->evidence_code_id);
    // pvalue
    $this->insert_feature_cvtermprop($feature_cvterm_id,
      $this->pvalue_cvterm_id, $pvalue);
    // analysis
    $this->insert_feature_cvtermprop($feature_cvterm_id,
      $this->analysis_cvterm_id, $this->analysis_id);
  }

  /**
   * Inserts a new entry into the feature_cvtermprop table and returns the id
   * of the new record.
   *
   * @param int $feature_cvterm_id
   *  The id of the feature cvterm with which to associate this cvtermprop.
   *
   * @param int $type_id
   *  Value to be inserted into the type_id column.
   * @param string $value
   *  Value to be inserted into the value column.
   * @param null $cvalue_id
   *  Optional parameter indicating an associated cvterm.
   *
   * @return \DatabaseStatementInterface|int
   *  The id of the inserted feature cvtermprop.
   *
   * @throws \Exception
   * @see tripal_expression_pvalue_loader::insert_feature_cvterm()
   *
   */
  private function insert_feature_cvtermprop(
    $feature_cvterm_id,
    $type_id,
    $value,
    $cvalue_id = NULL
  ) {
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
   * Inserts a new entry into the feature_cvterm table and returns the id of
   * the new record.
   *
   * @param int $feature_id
   *  The feature with which to associate this cvterm.
   *
   * @return \DatabaseStatementInterface|int
   *  The id of the inserted feature cvterm.
   * @throws \Exception
   */
  private function insert_feature_cvterm($feature_id, $rank) {
    $feature_cvterm_id = db_insert('chado.feature_cvterm')->fields([
      'feature_id' => $feature_id,
      'cvterm_id' => $this->cvterm_id,
      'pub_id' => $this->pub_id,
      'rank' => $rank,
    ])->execute();

    return $feature_cvterm_id;
  }

  /**
   * Get the ID of the cvterm with a known name.
   *
   * @param string $feature_name
   *
   * @return mixed
   *  Integer ID of the cvterm.
   * @throws \Exception
   */
  private function get_feature_id($feature_name, $organism_id, $type_id) {
    $feature_id = db_query('SELECT f.feature_id FROM {chado.feature} f 
        WHERE uniquename = :feature_name
        AND organism_id = :organism_id
        AND type_id = :type_id', [
      ':feature_name' => $feature_name,
      ':organism_id' => $organism_id,
      ':type_id' => $type_id,
    ])->fetchField();

    tripal_log($feature_id);

    if (empty($feature_id)) {
      throw new Exception(t('The feature !feature does not exist in the database for the organism and sequence type provided. Please verify that the feature exists and that the correct organism and sequence type has been provided.',
        ['!feature' => $feature_name]));
    }

    return $feature_id;
  }
}
