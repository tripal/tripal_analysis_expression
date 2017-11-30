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

abstract class analysis_expression_data_downloader {

  //The expression data returned by the mview
  protected $expression_data = "";


  /**
   * The output file URI.
   */
  protected $outfile = '';


  static function callback() {
    $feature_ids = $_GET['feature_ids'];
    $analysis_id = $_GET['analysis_id'];
    //TODO: Get and set the user id

    $downloader = new static();
    $downloader->load($feature_ids, $analysis_id);
    $downloader->create();
    $downloader->write();
    //alert drupal file is done!

  }


  function load($feature_ids, $analysis_id) {

    $sql = '';

    if ($feature_ids && $analysis_id) {

      //query the mview for all listed features.
      $sql = 'SELECT * FROM {expression_feature_all}
          WHERE  feature_id IN (:ids)
          AND analysis_id =  :analysis_id';
    }

    if (!$feature_ids) {
      //no feature ids supplied, retrieve all for this analysis
      //query the mview for all features associated with this analyses
      $sql = 'SELECT * FROM {expression_feature_all}
                WHERE  analysis_id IN (:ids)';
    }

    $expression_data = chado_query($sql, [
      ":ids" => $feature_ids,
      ":analysis_id" => $analysis_id,
    ]);
    dpm($expression_data);
    $this->expression_data = $expression_data;
  }

  function create() {

    global $user;
    // Make sure the user directory exists
    $user_dir = 'public://tripal/users/' . $user->id;
    if (!file_prepare_directory($user_dir, FILE_CREATE_DIRECTORY)) {
      $message = 'Could not access the directory on the server for storing this file.';
      watchdog('tripal', $message, [], WATCHDOG_ERROR);
      drupal_json_output([
        'status' => 'failed',
        'message' => $message,
        'file_id' => '',
      ]);
      return;
    }
    $this->outfile = $user_dir . '/expression_data.txt';
  }


  /**
   * Retrieves the URL for the downloadable file.
   */
  public function getURL() {
    return $this->outfile;
  }


  function write() {
    $expression_data = $this->expression_data;
    $out_file = $this->outfile;
    $data = [];
    $biomaterials = [];

    //loop through data.  need to build array with structure
    foreach ($expression_data as $result) {
      $feature = $result->feature;
      $biomaterial = $result->biomaterial;
      $value = $result->value;
      $data[$feature][$biomaterial] = $value;
      $biomaterials[$biomaterial] = $biomaterial;
    }


    $fh = fopen(drupal_realpath($out_file), "w");
    if (!$fh) {
      throw new Exception("Cannot open collection file: " . $out_file);
    }

    //prepare the header
    $header = implode("\t", $biomaterials);
    $header .= "\n";
    fwrite($fh, $header);

    foreach ($data as $feature) {
      foreach ($feature as $biomaterial => $value) {
        //TODO:  How do i deal with missing values?  If one feature doesnt have data for a biomaterial i want it to be NA....
        //i think we need to loop throguh data 2x, once to get a list of biomaterials and again to populate it with na's first then add the data if it exists
        fwrite($fh, $value . "\t");
      }

      fwrite($fh, "\n");//end of feature, newline
    }
    fclose($fh);


    //    $file = new stdClass();
    //    $file->uri = $this->outfile;
    //    $file->filename = basename($this->outfile);
    //    $file->filemime = file_get_mimetype($this->outfile);
    //    $file->uid = $user->uid;
    //    $file->status = FILE_STATUS_PERMANENT;
    //    $file = file_save($file);
    //    $fid = $file->fid;
    //    $file = file_load($fid);
    //    // We use the fid for the last argument because these files
    //    // aren't really associated with any entity, but we need a value.
    //    file_usage_add($file, 'tripal', 'data-collection', $fid);

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