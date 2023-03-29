<?php

//* We want to make sure we have a mview (s)
//
//* That has the field's query.
//* All features with expression data from an analysis ( i think we have this one)
//
//Download class is going to have...
//
//* A single feature
//* Multi features
//* Analysis
//
//Class needs write method
//
//Drupal method file_transfer
//
//copy the delete method
//
//menu callback
//get callback is going to have a type and whatever other parameters
//
//* create deals with filehandle
//* write just gets passed a path
//* delete #skip for now
//* load
//
//path should be `tripal/users/ . $users->uid . /tripal_analysis_expression/`
//

class analysis_expression_data_downloader{

  /**
   * The expression data returned by the mview
   *
   * @var array
   */
  public $expression_data = [];

  /**
   * The output file URI.
   *
   * @var string
   */
  protected $outfile = '';

  /**
   * An array of feature IDs that will be included in the download.
   *
   * @var array
   */
  protected $feature_ids = [];

  /**
   * A single analysis must be selected to compile and download expression data.
   *
   * @var int
   */
  protected $analysis_id = 0;

  /**
   * The callback to execute the download.
   *
   * @throws \Exception
   */
  public static function callback() {
    $downloader = new static();
    $downloader->load();
    $downloader->create();
    $downloader->write();
    $downloader->serve();
  }

  /**
   * Load the data.
   *
   * @throws \Exception
   */
  public function load() {
    $feature_ids = NULL;

    if (isset($_GET['feature_ids'])) {
      $this->feature_ids = explode(',', $_GET['feature_ids']);
    }

    $this->analysis_id = $_GET['analysis_id'];
    $feature_ids = $this->feature_ids;
    $analysis_id = $this->analysis_id;

    if ($feature_ids && $analysis_id) {
      // Query the mview for all listed features.
      $sql = 'SELECT * FROM {expression_feature_all}
              WHERE feature_id IN (:ids) AND analysis_id = :analysis_id';

      $keys = [":ids" => $feature_ids, ":analysis_id" => $analysis_id];
    }
    else {
      // No feature ids supplied, retrieve all for this analysis
      // Query the mview for all features associated with this analyses
      $sql = 'SELECT * FROM {expression_feature_all}
              WHERE analysis_id = :analysis_id';
      $keys = [":analysis_id" => $analysis_id];
    }

    $this->expression_data = chado_query($sql, $keys)->fetchAll();
  }

  /**
   * Create the downloadable file.
   */
  public function create() {
    // Make sure the user directory exists
    $user_dir = tripal_get_files_dir('tripal_analysis_expression_download');
    if (!file_prepare_directory($user_dir, FILE_CREATE_DIRECTORY)) {
      $message = 'Could not access the directory '.$user_dir.' for storing this file.';
      watchdog('tripal', $message, [], WATCHDOG_ERROR);
      return;
    }
    $this->outfile = $user_dir . '/expression_data_' . uniqid() . '.csv';
  }

  /**
   * Retrieves the URL for the downloadable file.
   */
  public function serve() {
    $uri = $this->outfile;
    $headers = ['Content-Type' => 'text/csv; utf-8'];
    if (ob_get_level()) {
      ob_end_clean();
    }

    foreach ($headers as $name => $value) {
      drupal_add_http_header($name, $value);
    }
    drupal_send_headers();
    $uri = drupal_realpath($uri);
    //$scheme = file_uri_scheme($uri);
    // Transfer file in 1024 byte chunks to save memory usage.
    //    if ($scheme && file_stream_wrapper_valid_scheme($scheme) && $fd = fopen($uri,
    if ($fd = fopen($uri,
        'rb')) {
      while (!feof($fd)) {
        print fread($fd, 1024);
      }
      fclose($fd);
    }
    else {
      drupal_not_found();
    }

    try {
      @unlink(drupal_realpath($this->outfile));
    } catch (Exception $exception) {
      tripal_log($exception->getMessage());
    }

    drupal_exit();
  }

  /**
   * Write data to file.
   *
   * @throws \Exception
   */
  public function write() {

    // drupal_realpath() is converting this file to null if the file does not exist.
    // split off the file name so that this doesn't happen.
    // $out_file = $this->outfile;
    $path_parts = pathinfo($this->outfile);
    $out_file = drupal_realpath($path_parts['dirname']) . '/' . $path_parts['basename'];

    $data = [];
    $biomaterials = [];

    // Loop through data. Need to build array with structure
    foreach ($this->expression_data as $result) {
      $data[$result->feature_uniquename][$result->biomaterial_name] = $result->signal;
      $biomaterials[$result->biomaterial_name] = $result->biomaterial_name;
    }

    $fh = fopen($out_file, "w");
    if (!$fh) {
      throw new Exception("Cannot open collection file: " . $out_file);
    }

    // Prepare the header
    $header = " ,";//blank for feature name
    $header .= implode(",", $biomaterials);
    $header .= "\n";

    fwrite($fh, $header);

    foreach ($data as $feature_name => $feature) {
      fwrite($fh, $feature_name . ",");
      // Loop through biomaterials (rows)
      foreach ($biomaterials as $biomaterial) {
        $value = "NA";
        if (isset($feature[$biomaterial])) {
          $value = $feature[$biomaterial];
        }
        fwrite($fh, $value . ",");
      }
      fwrite($fh, "\n"); // End of feature, newline
    }

    fclose($fh);
  }

  /**
   * skip for now
   */
  public function delete() {
    $fid = db_select('file_managed', 'fm')
      ->fields('fm', ['fid'])
      ->condition('uri', $this->outfile)
      ->execute()
      ->fetchField();
    if ($fid) {
      $file = file_load($fid);
      file_usage_delete($file, 'tripal', 'data-collection');
      file_delete($file, TRUE);
    }
  }
}
