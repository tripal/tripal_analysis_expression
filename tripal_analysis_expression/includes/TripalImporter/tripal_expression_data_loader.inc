<?php

/**
 * Class tripal_expression_data_loader
 *
 * @see TripalImporter
 */

class tripal_expression_data_loader extends TripalImporter {

  public static $name = 'Chado Expression Data Loader';

  public static $machine_name = 'Chado_Expression_Data_Loader';

  public static $description = 'Import expression data.  Requires an analaysis and biomaterial.';

  public static $file_types = ['csv', 'tsv', 'txt'];

  public static $upload_description = 'Expression data can be loaded from two format types. ' .
    'Select the column format for files that have two columns - transcript id and expression ' .
    'value. Select the matrix format for files that specify biosample ' .
    'by column and transcript by row. If "Column Format" is selected, the name of the column ' .
    'file will be taken as the biomaterial name. It is recommended to avoid the use of white ' .
    'space in column file names and in biomaterial names.<br><br>Please verify the column or file ' .
    'names match the intended biomaterial name in the database. The uploaded file cannot be a directory.';

  public static $upload_title = 'Upload expression data';

  public static $use_analysis = TRUE;

  public static $require_analysis = TRUE;

  public static $button_text = 'Import expression data';

  public static $methods = [
    'file_upload' => TRUE,
    'file_local' => FALSE,
    'file_remote' => FALSE,
  ];

  public static $file_required = TRUE;
  
  /**
   * The organism ID that the network belongs to.
   */
  private $organism_id = NULL;
  
  /**
   * The organism ChadoRecord object that corresponds to the $organism_id value.
   */
  private $organism = NULL;
  
  /**
   * The analysis ID for this GFF file
   */
  private $analysis_id = NULL;
  
  /**
   * The analysis ChadoRecord object that corresponds to the $analysis_id value.
   */
  private $analysis = NULL;
  
  /**
   * The associative array for holding information about the samples.
   */
  private $samples = [];
  /**
   * The associative array for holding information about the features..
   */
  private $features = [];

  /**
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

    $organisms = chado_get_organism_select_options(FALSE);

    $form['organism_id'] = [
      '#title' => t('Organism'),
      '#type' => t('select'),
      '#required' => TRUE,
      '#description' => t('The organism from which the biosamples were collected.'),
      '#options' => $organisms,
    ];

    $cv = tripal_get_cv(['name' => 'sequence']);
    $cv_id = $cv->cv_id;

    $form['seqtype'] = [
      '#type' => 'textfield',
      '#title' => t('Sequence Type'),
      '#required' => TRUE,
      '#description' => t('Please enter the Sequence Ontology (SO) term name that describes the sequences in the expression file (e.g. gene, mRNA, polypeptide, etc...).  Only necessary if the uniquename does not uniquely identify the feature for this organism.'),
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/$cv_id",
    ];
    
    $form['missing_value'] = [
      '#type' => 'textfield',
      '#title' => t('Missing Value String (Matrix Format)'),
      '#required' => FALSE,
      '#description' => t('Provide the text used to represent missing values in your input matrix file. Common examples include: NA, 0, -Inf. Entries with missing values will not be imported.'),
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

    $lt_message = tripal_set_message(t('If the array design does not appear in this list, ') . l(t('add a new arraydesign.'),
        'bio_data/add') . t(' If an array design is not selected, then a generic array design will be created for the analysis.'),
      TRIPAL_INFO, ['return_html' => TRUE]);

    $arraydesigns = tripal_analysis_expression_get_arraydesign_select_options();

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

    $lt_message = tripal_set_message(t('If the acquisition protocol is not present in the dropdown list, ') . l(t('add a new protocol.'),
        'bio_data/add') . t(' An acquisition protocol is not required.'),
      TRIPAL_INFO, ['return_html' => TRUE]);

    $acquisitions = tripal_analysis_expression_get_protocol_select_options('Acquisition Protocol');

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
      '#default_value' => 'col',
    ];

    $form['feature_uniquenames'] = [
      '#type' => 'radios',
      '#title' => t('Name Match Type'),
      '#description' => t('Expression data can be associated with features via the feature name or the feature unique name.'),
      '#options' => [
        'name' => t('Name'),
        'uniquename' => t('Unique name'),
      ],
      '#required' => TRUE,
    ];

    $form['delimiter'] = [
      '#type' => 'select',
      '#title' => t('Data Delimieter'),
      '#options' => [
        'tab' => 'tab',
        'comma' => 'comma'
      ],
      '#description' => t('Specify if the data in the input file is comma or tab separated.'),
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

    $re_start = $form_state['values']['re_start'];
    $re_end = $form_state['values']['re_end'];
    $seqtype = isset($form_state['values']['seqtype']) ? $form_state['values']['seqtype'] : NULL;

    $identifier = [
      'cv_id' => ['name' => 'sequence'],
      'name' => $seqtype,
    ];
    $type = chado_get_cvterm($identifier);
    if (!$type) {
      form_set_error('seqtype',
        'Please provide a valid parent type (or none at all)');
    }
    
    // Validate the regular expressions if provided.
    if ($re_start and @preg_match('/' . $re_start . '/', NULL) === FALSE) {
      form_set_error('re_start',
        'Invalid regular expression for the start of expression data: ' . $re_start);
    }
    if ($re_end and @preg_match('/' . $re_end . '/', NULL) === FALSE) {
      form_set_error('re_end',
        'Invalid regular expression for the end of expression data: ' . $re_end);
    }
  }
  
  /**
   * Opens the cache file for read/write access.
   */
  private function openCacheFile() {
    $temp_file = drupal_tempnam('temporary://', "TripalExpressionImport_");
    $this->cache_file_name = drupal_realpath($temp_file);
    $this->logMessage("Opening temporary cache file: !cfile",
        ['!cfile' => $this->cache_file_name]);
    $this->cache_file = fopen($this->cache_file_name, "r+");
  }
  
  /**
   * Closes and cleans up the cache file.
   */
  private function closeCacheFile() {
    fclose($this->cache_file);
    $this->logMessage("Removing temporary cache file: !cfile",
        ['!cfile' => $this->cache_file_name]);
    unlink($this->cache_file_name);
  }
  
  /**
   * Caches the processed feature from a GFF3 file
   */
  private function cacheFeature($feature) {
    // Make sure we're at the end of the file.
    fseek($this->cache_file, 0, SEEK_END);
    
    // Get the index of this location
    $findex = ftell($this->cache_file);
    
    // Write the serialied array for this feature to the cache file
    // and save the index into the member variable.
    fwrite($this->cache_file, serialize($feature) . "\n");
    $this->features[$feature['name']]['findex'] = $findex;
    $this->features[$feature['name']]['feature_id'] = NULL;
  }
  
  /**
   * Caches the processed feature from a GFF3 file
   */
  private function cacheSample($sample) {
    // Make sure we're at the end of the file.
    fseek($this->cache_file, 0, SEEK_END);
    
    // Get the index of this location
    $findex = ftell($this->cache_file);
    
    // Write the serialied array for this feature to the cache file
    // and save the index into the member variable.
    fwrite($this->cache_file, serialize($sample) . "\n");
    $this->samples[$sample['name']]['findex'] = $findex;
    $this->samples[$sample['name']]['biomaterial_id'] = NULL;
  }
  
  /**
   * Retrieves an item from the cache.
   */
  private function getCachedItem($findex) {
    $retval = fseek($this->cache_file, $findex);
    if ($retval == -1) {
      throw new Exception(t('Cannot seek to file location, !findex, in cache file !file.',
          ['!findex' => $findex, '!file' -> $this->cache_file]));
    }
    $item = fgets($this->cache_file);
    $item = unserialize($item);
    return $item;
  }  
  
  
  /**
   *
   */
  private function openFile() {
    $arguments = $this->arguments['run_args'];
    $filepath = $this->arguments['files'][0]['file_path'];
    
    $this->file = fopen($filepath, "r");
  }
  /**
   *
   */
  private function closeFile(){
    fclose($this->file);
  }
  /**
   *
   */
  private function parseFile() {   
    $arguments = $this->arguments['run_args'];
    $filetype = $arguments['filetype'];
    if ($filetype == "col") {
      $this->parseColumFormat();
    }
    else {
      $this->parseMatrixFormat();
    }
  }
  
  /**
   * 
   */
  private function parseColumnFormat() {
    
    
  }
  
  /**
   * Retreives the next line from the input file.
   * 
   * It returns the line and the start position in the file 
   * for the line. We'll store the start position for each
   * line so that we don't have to store the data file in 
   * memory as some can get rather lanrge.
   */
  private function getNextLine() {
    $findex = ftell($this->file);
    $line = fgets($this->file);
    if (!$line) {
      return [];
    }
    return [
      'findex' => $findex, 
      'line' => trim($line)     
    ];
  }
  /**
   */
  private function parseMatrixFormat() {
    $arguments = $this->arguments['run_args'];
    $re_start = $arguments['re_start'];
    $re_stop = $arguments['re_stop']; 
    $delim = $arguments['delimiter'] == 'tab' ? "\t" : ',';
        
    // Get the header
    $in_header = $re_start ? FALSE: TRUE;
    $in_data = FALSE;
    $in_footer = TRUE;
    $num_lines = 0;
    $row = 0;
    $num_samples = 0;
    
    while ($data = $this->getNextLine()) {
      $line = $data['line'];
      $findex = $data['findex'];      
      $num_lines++;
      
      // Skip lines before the header
      if (!$in_header and !$in_data and !$in_footer and preg_match('/' . $re_start . '/', $line)) {
        $in_header = TRUE;
        $findex = ftell($this->file);
        continue;
      }            
      
      // If we've hit the header then extract the sample names.
      if ($in_header) {
        $sample_names = explode($delim, $line); 
        $num_samples = count($sample_names);
        foreach ($sample_names as $index => $sample_name) {
          $this->samples[$sample_name] = [
            'name' => $sample_name,
            'file_column' => $index            
          ];
        }
        $in_header = FALSE;
        $in_data = TRUE;
        $findex = ftell($this->file);
        continue;
      }
        
      if ($in_data) {        
        // Skip unwanted footer lines lines.
        if ($re_stop and preg_match('/' . $re_stop . '/', $line)) {
          $in_footer = TRUE;
          $in_data = FALSE;
          $findex = ftell($this->file);
          continue;
        }
        
        // Get the line of data.
        $values = explode($delim, $line);
        $feature_name = array_shift($values);
        $feature = [
          'name' => $feature_name,
          'file_row' => $row,
          'emx_findex' => $findex
        ];
        $this->features[$feature_name] = $feature;
        $this->cacheFeature($feature);
        $row++;
        
        // As a sanity check, make sure the number of values matches the number
        // of samples.
        if (count($values) != $num_samples) {
          throw new Exception(t('Line, !num_lines, of the input matrix has an incorrect number of values.',
              ['!num_lines' => $num_lines]));
        }
        $findex = ftell($this->file);
      }
      
      // Skip any remaining lines in the footer of the file.
      if ($in_footer) {
        continue;
      }
    }
  }
  
  /**
   * Retreives sample information from NCBI
   */
  private function findNCBISamples() {
    $sra_ids = [];

    // First iterate through the samples and for 
    // any that have an NCBI experiment or run ID accession 
    // number look up the sample.
    foreach ($this->samples as $sample) {
      $sample_name = $sample['name'];
      // SRA Experiment IDs
      if (preg_match('/^[SDE]RX\d+$/', $sample_name)) { 
        $sra_ids[] = $sample_name;
      }
      // SRA Run IDs
      elseif (preg_match('/^[SDE]RR\d+$/', $sample['name'])) {
        $sra_ids[] = $sample_name;
      }
      // GEO IDs
      elseif (preg_match('/^GSM\d+$/', $sample['name'])) {
        $sra_ids[] = $sample_name;
      } 
    }
    
    $batch_size = 100;
    $num_samples = count($sra_ids);
    $num_batches = (int) ($num_samples / $batch_size) + 1;
        
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    for ($i = 0; $i < $num_batches; $i++) {   
      $ids = array_slice($sra_ids, min($batch_size * $i, $num_samples), $batch_size);
      $url = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?retmod=xml&db=sra&id=" . join(',', $ids);
      $response = drupal_http_request($url, []);
      if (!empty($response)) {
        // Parse the response.        
        $xml = new XMLReader();
        $xml->XML($response->data);
        while ($xml->read()) {
          if ($xml->nodeType == XMLReader::ELEMENT) {
            if (strcmp($xml->name, 'EXPERIMENT_PACKAGE_SET') == 0) {
              $experiments = $this->parseExpPackageSetXML($xml);
            }
          }
        }
      }
      
      foreach ($experiments as $experiment) {
        foreach ($experiment as $sra_id => $sample_ids) {
          if (array_key_exists($sra_id, $this->samples)) {
            $this->samples[$sra_id]['sra_sample_ids'] = $sample_ids;
          }
        }
      }          
      // Now map 
      $this->setItemsHandled($i);
    }    
  }
  
  /**
   * Parses the EXPERIMENT_PACKAGE_SET element in the NCBI XML
   */
  private function parseExpPackageSetXML($xml){
    $experiments = [];
    while ($xml->read()) {
      if ($xml->nodeType == XMLReader::ELEMENT) {
        if (strcmp($xml->name, 'EXPERIMENT_PACKAGE') == 0) {
          $experiments[] = $this->parseExpPackageXML($xml);
        }
      }
      // If we're at the </BioSample> element then we're done.
      elseif ($xml->nodeType == XMLReader::END_ELEMENT) {
        if (strcmp($xml->name, 'EXPERIMENT_PACKAGE_SET') == 0) {
          break;
        }
      }
    }
    return $experiments;
  }
  /**
   * Parses the EXPERIMENT_PACKAGE element in the NCBI XML
   */
  private function parseExpPackageXML($xml){
    $experiment = [];
    $exp_id = NULL;
    $alias_id = NULL;
    while ($xml->read()) {
      if ($xml->nodeType == XMLReader::ELEMENT) {
        if (strcmp($xml->name, 'EXPERIMENT') == 0) {
          $exp_id = $xml->getAttribute('accession');
          $alias_id = $xml->getAttribute('alias');
        }
        if (strcmp($xml->name, 'SAMPLE') == 0) {
          $sample_ids = $this->parseSampleXML($xml);
        }
        if (strcmp($xml->name, 'RUN_SET') == 0) {
          $run_ids = $this->parseRunSetXML($xml);
        }
      }
      // If we're at the </BioSample> element then we're done.
      elseif ($xml->nodeType == XMLReader::END_ELEMENT) {
        if (strcmp($xml->name, 'EXPERIMENT_PACKAGE') == 0) {
          break;
        }
      }
    }
    
    // Make it easy to lookup the sample IDs by 
    // experiment ID, GEO ID (alias) or RUN ID.
    $experiment[$exp_id] = $sample_ids;
    if ($alias_id) {
      $experiment[$alias_id] = $sample_ids;
    }    
    foreach ($run_ids as $run_id) {
      $experiment[$run_id] = $sample_ids;
    }
    return $experiment;
  }
  /**
   * Parses the RUN_SET element in the NCBI XML
   */
  private function parseRunSetXML($xml){
    $exp_run_ids = [];
    while ($xml->read()) {
      if ($xml->nodeType == XMLReader::ELEMENT) {
        if (strcmp($xml->name, 'RUN') == 0) {
          $run_ids = $this->parseRunXML($xml);
          $exp_run_ids = array_merge($exp_run_ids, $run_ids);
        }
      }
      // If we're at the </BioSample> element then we're done.
      elseif ($xml->nodeType == XMLReader::END_ELEMENT) {
        if (strcmp($xml->name, 'RUN_SET') == 0) {
          break;
        }
      }
    }
    return $exp_run_ids;
  }
  /**
   * Parses the RUN element in the NCBI XML
   */
  private function parseRunXML($xml){
    while ($xml->read()) {
      if ($xml->nodeType == XMLReader::ELEMENT) {
        if (strcmp($xml->name, 'IDENTIFIERS') == 0) {
          $ids = $this->parseIDsXML($xml);
        }
      }
      // If we're at the </BioSample> element then we're done.
      elseif ($xml->nodeType == XMLReader::END_ELEMENT) {
        if (strcmp($xml->name, 'RUN') == 0) {
          break;
        }
      }
    }
    return $ids;
  }
  /**
   * Parses the SAMPLE element in the NCBI XML
   */
  private function parseSampleXML($xml){
    $sample_ids = [];
    while ($xml->read()) {
      if ($xml->nodeType == XMLReader::ELEMENT) {
        if (strcmp($xml->name, 'IDENTIFIERS') == 0) {
          $sample_ids = $this->parseIDsXML($xml);
        }
      }
      // If we're at the </BioSample> element then we're done.
      elseif ($xml->nodeType == XMLReader::END_ELEMENT) {
        if (strcmp($xml->name, 'SAMPLE') == 0) {
          break;
        }
      }
    }
    return $sample_ids;
  }
  /**
   * Parses the IDENTIFIERS element in the NCBI XML
   */
  private function parseIDsXML($xml){ 
    $ids = [];
    while ($xml->read()) {
      if ($xml->nodeType == XMLReader::ELEMENT) {
        if (strcmp($xml->name, 'PRIMARY_ID') == 0) {
          $xml->read();
          $ids[] = $xml->value;
        }
        if (strcmp($xml->name, 'EXTERNAL_ID') == 0) {
          $xml->read();
          $ids[] = $xml->value;
        }
      }
      // If we're at the </BioSample> element then we're done.
      elseif ($xml->nodeType == XMLReader::END_ELEMENT) {
        if (strcmp($xml->name, 'IDENTIFIERS') == 0) {
          break;
        }
      }
    }
    return $ids;
  }
  
 
  /**
   * Check if the features exist in the database.
   */
  private function findLocalSamples() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    $sql = "        
      SELECT name, biomaterial_id
      FROM {biomaterial}
      WHERE name in (:names)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $names = [];
    $mapping = [];
    foreach ($this->samples as $name => $info) {      
      $biomaterial_id = array_key_exists('biomaterial_id', $info) ? $info['biomaterial_id'] : NULL;
      $total++;
      
      if (!$biomaterial_id) {
        $i++;
        
        // If the expression sample is an SRA ID then we want
        // to properly link it up to it's biomaterial.  If we have
        // no SRA IDs then we'll assume the bio sample name and the
        // expression sample name are the same.
        if (array_key_exists('sra_sample_ids', $info)) {
          foreach ($info['sra_sample_ids'] as $sra_sample_id) {
            $names[] = $sra_sample_id;
            $mapping[$sra_sample_id] = $name;
          }
        }
        else {
          $names[] = $name;
          $mapping[$sra_sample_id] = $name;
        }
      }
      
      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($names) > 0) {
          $args = [':names' => $names];
          $results = chado_query($sql, $args);
          while ($b = $results->fetchObject()) {
            $this->samples[$mapping[$b->name]]['biomaterial_id'] = $b->biomaterial_id;
            $this->samples[$mapping[$b->name]]['biomaterial_name'] = $b->name;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $names = [];
        $mapping = [];
      }
    }
  }
  
  /**
   * Check if the features exist in the database.
   */
  private function findFeatures() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;
    $fname = $this->seqname;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $sql = "
      SELECT uniquename, name, feature_id 
      FROM {feature} 
      WHERE $fname in (:names) and type_id = :type_id and organism_id = :organism_id
    ";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $names = [];
    foreach ($this->features as $name => $info) {
      $feature_id = $info['feature_id'];
      $total++;
      
      if (!$feature_id) {
        $i++;
        $names[] = $name;
      }
      
      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_features) {
        if (count($names) > 0) {
          $args = [
            ':names' => $names,
            ':type_id' => $this->feature_type_id,
            ':organism_id' => $this->organism->getID(),
          ];
          $results = chado_query($sql, $args);
          while ($f = $results->fetchObject()) {
             $this->features[$f->$fname]['feature_id'] = $f->feature_id;
             $this->features[$f->$fname]['name'] = $f->name;
             $this->features[$f->$fname]['uniquename'] = $f->uniquename;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $names = [];
      }
    }
  }
  
  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $this->organism_id = $arguments['organism_id'];
    $this->analysis_id = $arguments['analysis_id'];
    $this->missing_val = $arguments['missing_value'];
    $this->seqtype = $arguments['seqtype'];
    $this->seqname = $arguments['feature_uniquenames'];
    $this->arraydesign_id = $arguments['arraydesign_id']; 
    $this->quantificationunits = $arguments['quantificationunits'];
    
    // Get the organism object.
    $this->organism = new ChadoRecord('organism');
    $this->organism->setValues(['organism_id' => $this->organism_id]);
    $num_found = $this->organism->find();
    if ($num_found == 0) {
      throw new Exception(t("Cannot find the specified organism."));
    }
    
    // Get the analysis object.
    $this->analysis = new ChadoRecord('analysis');
    $this->analysis->setValues(['analysis_id' => $this->analysis_id]);
    $num_found = $this->analysis->find();
    if ($num_found == 0) {
      throw new Exception(t("Cannot find the specified analysis."));
    }
    
    // Get the feature type.
    $sel_cvterm_sql = "
      SELECT CVT.cvterm_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV on CV.cv_id = CVT.cv_id
        LEFT JOIN {cvtermsynonym} CVTS on CVTS.cvterm_id = CVT.cvterm_id
      WHERE CV.name = :cv_name and
       (lower(CVT.name) = lower(:name) or lower(CVTS.synonym) = lower(:synonym))
    ";
    $result = chado_query($sel_cvterm_sql, [
      ':cv_name' => 'sequence',
      ':name' => $this->seqtype,
      ':synonym' => $this->seqtype,
    ]);
    $this->feature_type_id = $result->fetchField();

    // Create the cache file for storing parsed GFF entries.
    $this->openCacheFile();
    
    try {     
      $this->logMessage("Step  1 of 10: Caching input file...                             ");
      $this->openFile();
      $this->parseFile();
      $this->logMessage("  Found " . count(array_keys($this->features)) . " features and " . count(array_keys($this->samples)) . " samples.");
      
      $this->logMessage("Step  2 of 10: Find Features...                             ");
      $this->findFeatures();
      
      $this->logMessage("Step  3 of 10: Find NCBI Samples...                             ");
      $this->logMessage("  Note: this step may take a while. It queries NCBI if the samples use GEO or SRA IDs.");
      $this->findNCBISamples();
     
      $this->logMessage("Step  4 of 10: Find Local Samples...                            ");
      $this->findLocalSamples();
    }
    // On exception, catch the error, clean up the cache file and rethrow
    catch (Exception $e) {
      $this->closeCacheFile();
      $this->closeFile();
      throw $e;
    }
    $this->closeCacheFile();
    $this->closeFile();
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
   * @param $seqtype
   * Specify the feature type to associate the data with, or, NULL.
   *
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
    $create_biosamples,
    $seqtype,
    $missing_value
  ) {

    $seqtype_id = NULL;
    if ($seqtype) {
      $identifier = [
        'cv_id' => ['name' => 'sequence'],
        'name' => $seqtype,
      ];

      $seqtype_id = chado_get_cvterm($identifier)->cvterm_id;
    }

    //these should be passed in parameters but are optional and apparently not used.
    //BC
    $assay_id = NULL;
    $acquisition_id = NULL;
    $quantification_id = NULL;

    $features = [];

    $uniq_name = $this->get_unique_name($organism_id, $analysis_id);

    try {
      // Create the array design.
      $arraydesign_id = $arraydesign_id ? $arraydesign_id : create_generic_arraydesign(create_generic_contact());
      $re_ext = '/^.+\.(' . $fileext . ')/';

      // A matrix file.
      if ($filetype == 'mat' and is_file($filepath)) {
        $this->tripal_expression_load_mat_file($filepath, $fileext,
          $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id,
          $acquisition_id, $quantification_id, $quantificationunits, $re_start,
          $re_stop, $uniq_name, $feature_uniquenames, $seqtype_id, $missing_value);
      }

      // The column format.
      elseif ($filetype == 'col' and is_file($filepath) and preg_match($re_ext,
          $filepath)) {
        $features = $this->tripal_expression_load_col_file($filepath, $fileext,
          $arraydesign_id, $organism_id, $analysis_id, $contact_id, $assay_id,
          $acquisition_id, $quantification_id, $quantificationunits, $re_start,
          $re_stop, $uniq_name, $feature_uniquenames);
      }
      else { // Multiple files in the column format.

        $dir_handle = @opendir($filepath) or die("Unable to open $filepath");
        $total_files = count(glob($filepath . '/*.' . $fileext));
        $num_file = 0;

        // Load all files with of type $fileext in the directory.
        while ($file = readdir($dir_handle)) {
          if (preg_match('/(.+)\.' . $fileext . '$/', $file)) {
            $this->logMessage('File !num_file of !total_files: !file', [
              '!num_file' => $num_file + 1,
              '!total_files' => $total_files,
              '!file' => $file,
            ]);
            $new_features = $this->tripal_expression_load_col_file($filepath . '/' . $file,
              $fileext, $arraydesign_id, $organism_id, $analysis_id,
              $contact_id, $assay_id, $acquisition_id, $quantification_id,
              $quantificationunits, $re_start, $re_stop, $uniq_name, $feature_uniquenames);
            $num_file++;
            $old_features = $features;
            $features = (array_unique(array_merge($old_features,
              $new_features)));
          }
        }
      }

      // Associate features with an analysis.
      foreach ($features as $feature_id) {

        //TODO: THIS WENT IN THE WRONG PLACE I THINK

        if ($seqtype_id) {

          $query = chado_query('SELECT afanalysisfeature_id FROM {analysisfeature} af
INNER JOIN {feature} f ON f.feature_id = af.feature_id
WHERE af.analysis_id = :analysis_id
AND af.feature_id = :feature_id
AND f.type_id = :type_id', [
            ':analysis_id' => $analysis_id,
            ':feature_id' => $feature_id,
            ':type_id' => $seqtype_id,
          ]);
        }
        else {
          $query = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id',
            [
              ':analysis_id' => $analysis_id,
              ':feature_id' => $feature_id,
            ]);
        }
        $analysisfeature_id = $query->fetchField();
        if (!$analysisfeature_id) {
          $analysisfeature = chado_insert_record('analysisfeature', [
            'analysis_id' => $analysis_id,
            'feature_id' => $feature_id,
          ]);
          if ($analysisfeature == FALSE) {
            $this->logMessage('Unable to write to analysisfeature table', [],
              TRIPAL_ERROR);
            return;
          }
        }
      }

      $cache_loc = tripal_analysis_expression_cache_file($filepath);

      if (!$cache_loc) {
        $this->logMessage('Failed to cache file !filepath',
          ['!filepath' => $filepath], TRIPAL_ERROR);
        return;
      }

      if (!tripal_analysis_expression_add_file_as_prop($analysis_id,
        $cache_loc)) {
        $this->logMessage('Failed to add !cache_loc to analysisprop table',
          ['!cache_loc' => $cache_loc], TRIPAL_ERROR);
        return;
      };
    } catch (Exception $e) {
      $this->logMessage('!e', ['!e' => $e], TRIPAL_ERROR);
    }

    return;
  }

  function get_unique_name($organism_id, $analysis_id) {

    $uniq_string = " from ";
    $sql = "SELECT common_name FROM {organism} WHERE organism_id = :organism_id";
    $uniq_string .= chado_query($sql,
      [':organism_id' => $organism_id])->fetchField();
    $uniq_string .= " for ";
    $sql = "SELECT name FROM {analysis} WHERE analysis_id = :analysis_id";
    $uniq_string .= chado_query($sql,
      [':analysis_id' => $analysis_id])->fetchField();
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
   * @param $feature_uniquenames
   *
   * @param $type_id
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
    $feature_uniquenames,
    $type_id,
    $missing_value
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

    if ($num_lines < 2) {
      $this->logMessage("Error reading file. Please ensure that your file is readable and try again.",
        [], TRIPAL_ERROR);
      return;
    }

    // Make sure there not duplicate feature names.
    if (count($feature_repeats = array_diff_assoc($feature_array,
        array_unique($feature_array))) > 0) {
      $this->logMessage('There are multiple instance of the following feature(s) in the expression file:');
      foreach ($feature_repeats as $repeat) {
        $this->logMessage("  !repeat", ['!repeat' => $repeat], TRIPAL_ERROR);
      }
      $this->logMessage('Please make sure there is only one instance of each feature in the expression file.');
      return;
    }

    // Get rid of the feature_array.
    unset($feature_array);

    // Rewind the file.
    rewind($mat_fp);
    $mat_head = 0;

    $mat_fp = fopen($filepath, 'r');
    $load_limit = 500;

    // Calculate interval length.
    $int_length = .01 * $num_features;

    // The minimum interval length is 1.
    if ($int_length < 1) {
      $int_length = 1;
    }

    // Loop through every line in a file.

    $this->logMessage("Loading complete: 0%. Memory: !memory_usage bytes. \r",
      ['!memory_usage' => number_format(memory_get_usage())]);

    for ($j = 0; $j < $num_lines and $line = fgets($mat_fp); $j++) {

      //i moved this here since $timestart is only used in the print below, but unsure of its purpose- BC
      $time_start = microtime(TRUE);

      if ($j % $load_limit == 0) {
        $set_fp = ftell($mat_fp);
        fclose($mat_fp);
        $time_end = microtime(TRUE);
        if ($j != 0) {
          $this->logMessage('!lps lines per second',
            ['!lps' => $load_limit / ($time_end - $time_start)]);
        }
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
            $warning = FALSE;
            $quant_array[] = expression_create_biomaterial_structure($sample,
              $organism_id, $analysis_id, $contact_id, $arraydesign_id,
              $assay_id, $acquisition_id, $quantification_id,
              $quantificationunits, $uniq_name, $warning);
          }

          // Make sure there not duplicate biomaterial names.
          if (count($bio_repeats = array_diff_assoc($linepart,
              array_unique($linepart))) > 0) {
            $this->logMessage('There are multiple instance of the following biomaterial(s) in the expression file:');
            foreach ($bio_repeats as $repeat) {
              $this->logMessage('!repeat', ['!repeat' => $repeat]);
            }
            $this->logMessage('Please make sure there is only one instance of each biomaterial in the expression file');
            return;
          }

          $num_biomaterials = count($linepart);
          $mat_head = 1;
          continue;
        }
        else {

          // Make sure that there are enough expression columns.
          if ($num_col != ($num_biomaterials + 1)) {
            $this->logMessage("Expression data in the data section of the matrix format must have a feature name followed by tab separated expression values. The number of expression values for each feature must equal the number of biomaterials in the matrix file header. Example matrix format:\n" . "  <feature name>       <expression value>	<expression value> ...");
            if (count($linepart) > 0) {
              $this->logMessage("Loader failed on the following line:\n !line\n. Expected !expected_columns columns but only found !actual_columns column(s).",
                [
                  '!line' => $line,
                  '!expected_columns' => $num_biomaterials + 1,
                  '!actual_columns' => $num_col,
                ], TRIPAL_ERROR);
            }
            else {
              $this->logMessage('Loader failed because of a blank line in the data section of the expression file.',
                [], TRIPAL_ERROR);
            }
            return;
          }

          if ($cur_feature % $int_length == 0) {
            $percent = sprintf("%.2f", ($cur_feature / $num_features) * 100);
            $this->logMessage("Loading complete: !percent%. Memory: !memory_usage bytes. \r",
              [
                '!percent' => $percent,
                '!memory_usage' => number_format(memory_get_usage()),
              ]);
          }

          $cur_feature++;
          for ($i = 1; $i < $num_col; $i++) {
            $feature_id = $this->tripal_expression_find_feature_id($linepart[0],
              $organism_id, $feature_uniquenames, $type_id);
            if (!$feature_id) {
              tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
                "Feature $linepart[0] not found");
              continue;
            }
            $element_id = tripal_expression_element($arraydesign_id,
              $feature_id);
            tripal_expression_elementresult($element_id, $quant_array[$i - 1],
              $linepart[$i], $missing_value);
            // Create a record in the analysisfeature table. This will allow the analysis to appear in the
            // analysis list on the feature page.
            if ($i == 1) {
              $analysisfeature_id = chado_query('SELECT analysisfeature_id FROM {analysisfeature} WHERE analysis_id = :analysis_id AND feature_id = :feature_id',
                [
                  ':analysis_id' => $analysis_id,
                  ':feature_id' => $feature_id,
                ])->fetchField();
              if (!$analysisfeature_id) {
                $analysisfeature = chado_insert_record('analysisfeature', [
                  'analysis_id' => $analysis_id,
                  'feature_id' => $feature_id,
                ]);
                if ($analysisfeature == FALSE) {
                  tripal_report_error('tripal_analysis_expression',
                    TRIPAL_ERROR,
                    "ERROR: Unable to write to analysisfeature table");
                  continue;
                }
              }
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

    $quantification_id = expression_create_biomaterial_structure($biomaterial[1],
      $organism_id, $analysis_id, $contact_id, $arraydesign_id, $assay_id,
      $acquisition_id, $quantification_id, $quantificationunits, $uniq_name,
      TRUE);

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
    if (count($feature_repeats = array_diff_assoc($feature_array,
        array_unique($feature_array))) > 0) {
      //if (count($linepart) != count(array_unique($linepart))) {
      $this->logMessage('There are multiple instance of the following feature(s) in the expression file:');
      foreach ($feature_repeats as $repeat) {
        $this->logMessage("  !repeat", ['!repeat' => $repeat], TRIPAL_ERROR);

        $this->logMessage('Please make sure there is only one instance of each feature in the expression file.');
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

      $this->logMessage("Loading complete: 0%. Memory: !memory_usage bytes. \r",
        ['!memory_usage' => number_format(memory_get_usage())]);

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
            $this->logMessage("Loading complete: !percent%. Memory: !memory_usage bytes. \r",
              [
                '!percent' => $percent,
                '!memory_usage' => number_format(memory_get_usage()),
              ]);
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
            $this->logMessage("ERROR: Expression data in the data section of the column format must have two columns separated by a tab. Example column format:\n <feature name>	<expression value>");
            if (count($linepart) > 0) {
              $this->logMessage("Loader failed on the following line:\n !line",
                ['!line' => $line]);
            }
            else {
              $this->logMessage('Loader failed because of a blank line in the data section of the expression file.',
                [], TRIPAL_ERROR);
            }
            return;
          }

          $num_col = count($linepart);

          // Extract the feature id.
          $start = microtime(TRUE);
          $feature_id = $this->tripal_expression_find_feature_id($linepart[0],
            $organism_id, $feature_uniquenames);

          if (!$feature_id) {
            $this->logMessage('Unable to find feature: !feature',
              ['!feature' => $linepart[0]], TRIPAL_ERROR);
            continue;
          }

          // Add to feature list;
          $features[] = $feature_id;

          //print "Find feature_id: ";
          $f_id += microtime(TRUE) - $start . "\n";

          $start = microtime(TRUE);
          $element_id = tripal_expression_element($arraydesign_id, $feature_id);
          //print "Create element_id: ";
          $e_id += microtime(TRUE) - $start . "\n";

          $start = microtime(TRUE);
          tripal_expression_elementresult($element_id, $quantification_id,
            $linepart[1]);
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
  }

  /**
   *
   * Implements a function that find the find the feature_id from a feature
   * name.
   *
   * @param $feature
   *  The name of the feature to be found.
   *
   * @param $organism_id
   * The organism to find it in
   *
   * @param $feature_uniquenames
   *
   * @param $type_id
   * The type_id of the feature.  Can be Null.
   *
   * @ingroup tripal_analysis_expression
   */

  function tripal_expression_find_feature_id(
    $feature,
    $organism_id,
    $feature_uniquenames,
    $type_id = NULL
  ) {
    $query = db_select('chado.feature', 'f');
    $query->fields('f', ['organism_id', 'feature_id']);

    if ($feature_uniquenames == 'uniq') {
      $query->condition('f.uniquename', $feature);
    }
    else {
      $query->condition('f.name', $feature);
    }

    if ($type_id) {
      $query->condition('f.type_id', $type_id);
    }

    $count = (int) $query->countQuery()->execute()->fetchField();

    if ($count > 1) {
      $message = "ERROR: More than one feature matches the feature name: !feature.\n
      Please specify a term for the feature type.\n";
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
        $message, ['!feature' => $feature], TRIPAL_ERROR);
      return FALSE;
    }

    if ($count === 0) {
      tripal_report_error('tripal_analysis_expression', TRIPAL_WARNING,
        "The feature, " . $feature . ", found in the expression file was not found in the Chado database.
       Please ensure that the feature has been loaded into the database and that the feature name is both unique and correct.");
      return FALSE;
    }

    $feature_object = $query->execute()->fetchObject();

    if ($feature_object->organism_id != $organism_id) {
      $sql = "SELECT common_name, genus, species FROM {organism} WHERE organism_id = :organism_id";
      $analysis_org = chado_query($sql,
        [":organism_id" => $organism_id])->fetchObject();
      $feature_org = chado_query($sql,
        [":organism_id" => $feature_object->organism_id])->fetchObject();
      $analysis_org_sciname = $analysis_org->genus . ' ' . $analysis_org->species;
      $feature_org_sciname = $feature_org->genus . ' ' . $feature_org->species;

      $this->logMessage("Mismatch between the organism selected in the expression analysis and the organism of a feature from the expression file.\n
        Analysis organism: !analysis_org_sciname (!a_common_name)\n
        Feature organism: !feature_org_sciname (!f_common_name)\n
        On feature: !feature\n
        Please ensure that the organism selected for the expression analysis and the organism for each feature in the expression file are the same.\n",
        [
          '!analysis_org_sciname' => $analysis_org_sciname,
          '!feature_org_sciname' => $feature_org_sciname,
          '!a_common_name' => $analysis_org->common_name,
          '!f_common_name' => $feature_org->common_name,
          '!feature' => $feature,
        ], TRIPAL_ERROR);
      return FALSE;
    }

    return $feature_object->feature_id;
  }
}
