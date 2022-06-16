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
   * Indicates the number of files that can be uploaded. 0 means unlimited.
   */
  public static $cardinality = 0;
    
  /**
   * The ChadoRecord object that corresponds to the organism.
   */
  private $organism = NULL;
  
  /**
   * The ChadoRecord object that corresponds to the arraydesign..
   */
  private $ararydesign = NULL;
    
  /**
   * The ChadoRecord object that corresponds to the $analysis.
   */
  private $analysis = NULL;
  
  /**
   * The associative array for holding information about the samples.
   */
  private $samples = [];
  
  /**
   * The order of the samples in the input matrix file.
   */
  private $sample_order = [];
  
  /**
   * The associative array for holding information about the features..
   */
  private $features = [];
  
  
  /**
   * The string used to indicate a missing value.
   */
  private $missing_value = NULL;
  
  /**
   * The feature type (e.g., gene, mRNA, etc.)
   */
  private $seqtype = NULL;
  
  /**
   * Indicates the column in the feature table to use to match 
   * features: name, or uniquename. 
   */
  private $seqname = NULL;
  
  
  /**
   * The name of the quantification units, e.g., FPKM, RPKM, etc.
   */
  private $quantificationunits = NULL;
  
  /**
   * The type of file: 'col' (for column files) or 'mat' (for matrix file)
   */
  private $filetype = NULL;
  
  /**
   * The file delimiter: comma or tab.
   */
  private $delim = NULL;
  
  /**
   * The flag indicating if BioMaterials should be created.
   */
  private $create_biomaterials = NULL;
  
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
      '#title' => t('Missing Value'),
      '#required' => FALSE,
      '#description' => t('If your files have missing values, provide the text used to represent missing values in your input file. Common examples include: NA, 0, -Inf. Entries with missing values will not be imported.'),
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
    $sql = 'SELECT contact_id, name FROM {contact} ORDER BY name';
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
      '#title' => t('Data Delimiter'),
      '#options' => [
        'tab' => 'tab',
        'comma' => 'comma'
      ],
      '#description' => t('Specify if the file is comma or tab separated.'),
    ];
    
    $form['create_biomaterials'] = [
      '#type' => 'checkbox',
      '#title' => t('Create BioSample Records'),
      '#description' => t('Check here if the expression sample names in your input file(s) ' . 
         'should be used to create BioSample records?  The name of the expression sample will '. 
         'be used to create the biological sample. The BioSample is a "sample" of the biological ' .
         'source material (e.g. sample of tissue from an individual) and all expression ' . 
         'values are obtained from a biological sample. This loader links all expression samples with ' .
         'biololgical samples. NCBI distinguishes between expression samples and biological samples and provides different '.
         'IDs for them.  If your biological samples and expression samples use the same name and '.
         'if the biological samples are not already in the database you can check this box to ' .
         'have them created.  However, if you are using NCBI SRA or GEO accessions for expression samples, ' .
         'or if you would like attributes (metadata) about those biosamples included, use the ' . 
          l('Chado Biological Sample Loader', 'admin/tripal/loaders/chado_biosample_loader') .
         ' prior to this loader to load the biological samples first.'),
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
   *
   */
  private function openFiles() {
    foreach ($this->arguments['files'] as $info) {
      $this->files[] = fopen($info['file_path'], "r"); 
      $this->files_pathinfo[] = pathinfo($info['file_path']);;
    }
  }
  /**
   *
   */
  private function closeFiles(){
    foreach ($this->files as $fh) {
      fclose($fh);
    }
  }
  /**
   *
   */
  private function parseFiles() {   
    if ($this->filetype == "col") {
      $this->parseFilesColumns();
    }
    else {
      $this->parseFilesMatrix();
    }
  }
  
  /**
   * Retreives the next line from the input file.
   *
   * It returns the line and the start position in the file
   * for the line. We'll store the start position for each
   * line so that we don't have to store the data file in
   * memory as some can get rather lanrge.
   */
  private function getNextColumnLine($fh_index) {
    $findex = ftell($this->files[$fh_index]);
    $line = fgets($this->files[$fh_index]);
    if (!$line) {
      return [];
    }
    return [
      'findex' => $findex,
      'line' => trim($line)
    ];
  }
  
  /**
   * 
   */
  private function parseFilesColumns() {
    
    // Iterate through all of the files
    foreach ($this->files as $fh_index => $fh) {
      
      // The file names becaome the expression sample name.
      $sample_name = $this->files_pathinfo[$fh_index]['filename'];
      $this->sample_order[] = $sample_name;
      $this->samples[$sample_name] = [
        'name' => $sample_name,
        'fh_index' => $fh_index,
        'col' => $fh_index
      ];
      
      $num_lines = 0;
      while ($data = $this->getNextColumnLine($fh_index)) {
        $line = $data['line'];
        $findex = $data['findex'];
        $num_lines++;
        $values = explode($this->delim, $line);
        
        // Sanity check: make sure the line only has two columns
        if (count($values) != 2) {
          throw new Exception(t('Line, !num_lines, of the input file, !file, has an incorrect number of values. It has !has but should have 2. Check that the file delimiter has been set correctly and if the file is complete.',
              ['!num_lines' => $num_lines, 
                '!file' => $this->files_pathinfo[$fh_index]['basename'],
                '!has' => count($values),
              ]));
        }
        $feature_name = array_shift($values);
        if (!array_key_exists($feature_name, $this->features)) {
          $feature = [];
          $feature['name'] = $feature_name;
          $feature['findex'] = [];
          $feature['findex'][$fh_index] = $findex;
          $this->features[$feature_name] = $feature;
        }
        else {
          $this->features[$feature_name]['findex'][$fh_index] = $findex;
        }
      }            
    }    
  }
  
  /**
   * Retreives the next line from the input file.
   * 
   * It returns the line and the start position in the file 
   * for the line. We'll store the start position for each
   * line so that we don't have to store the data file in 
   * memory as some can get rather lanrge.
   */
  private function getNextMatrixLine() {
    $findex = ftell($this->files[0]);
    $line = fgets($this->files[0]);
    if (!$line) {
      return [];
    }
    return [
      'findex' => $findex, 
      'line' => rtrim($line)
    ];
  }
  /**
   */
  private function parseFilesMatrix() {
    $arguments = $this->arguments['run_args'];
    $re_start = $arguments['re_start'];
    $re_stop = $arguments['re_stop']; 
        
    // Get the header
    $in_header = $re_start ? FALSE: TRUE;
    $in_data = FALSE;
    $in_footer = TRUE;
    $num_lines = 0;
    $row = 0;
    $num_samples = 0;
    
    while ($data = $this->getNextMatrixLine()) {
      $line = $data['line'];
      $findex = $data['findex'];      
      $num_lines++;
      
      // Skip lines before the header
      if (!$in_header and !$in_data and !$in_footer and preg_match('/' . $re_start . '/', $line)) {
        $in_header = TRUE;
        $findex = ftell($this->files[0]);
        continue;
      }            
      
      // If we've hit the header then extract the sample names.
      if ($in_header) {
        $sample_names = explode($this->delim, $line); 
        // First column is feature_name, ignore this header.
        array_shift($sample_names);
        $this->sample_order = $sample_names;
        $num_samples = count($sample_names);
        foreach ($sample_names as $index => $sample_name) {
          $this->samples[$sample_name] = [
            'name' => $sample_name,
            'col' => $index            
          ];
        }
        $in_header = FALSE;
        $in_data = TRUE;
        $findex = ftell($this->files[0]);
        continue;
      }
        
      if ($in_data) {        
        // Skip unwanted footer lines lines.
        if ($re_stop and preg_match('/' . $re_stop . '/', $line)) {
          $in_footer = TRUE;
          $in_data = FALSE;
          $findex = ftell($this->files[0]);
          continue;
        }
        
        // Get the line of data.
        $values = explode($this->delim, $line);
        $feature_name = array_shift($values);
        $feature = [
          'name' => $feature_name,
          'findex' => $findex,
          'row' => $row,          
        ];
        $this->features[$feature_name] = $feature;
        $row++;
        
        // As a sanity check, make sure the number of values matches the number
        // of samples.
        if (count($values) != $num_samples) {
          throw new Exception(t('Line !num_lines of the input matrix has an incorrect number of values. !found values found, but !expected values were expected. Check that the file delimiter has been set correctly and if the file is complete.',
              ['!num_lines' => $num_lines,
               '!found' => count($values),
               '!expected' => $num_samples]));
        }
        $findex = ftell($this->files[0]);
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

    // If no NCBI identifiers are present, there is nothing to do.
    if (!$num_samples) {
      return;
    }
        
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
  private function findAssays() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    $sql = "
      SELECT assay_id, name
      FROM {assay}
      WHERE name in (:names)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $names = [];
    $mapping = [];
    foreach ($this->samples as $name => $info) {
      $total++;
      $i++;
      
      $assay_name =  $this->getAssayName($info);
      $names[] = $assay_name;
      $mapping[$assay_name] = $name;
              
      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($names) > 0) {
          $args = [':names' => $names];
          $results = chado_query($sql, $args);
          while ($b = $results->fetchObject()) {
            $sname = $mapping[$b->name];
            $this->samples[$sname]['assay_id'] = $b->assay_id;
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
  private function findAssayBiomaterials() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    $sql = "
      SELECT assay_biomaterial_id, assay_id, biomaterial_id
      FROM {assay_biomaterial}
      WHERE assay_id in (:assay_ids)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $assay_ids = [];
    $mapping = [];
    foreach ($this->samples as $name => $info) {
      $total++;
      $i++;
      
      $assay_ids[] = $info['assay_id'];
      $mapping[$info['assay_id']] = $name;
      
      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($assay_ids) > 0) {
          $args = [':assay_ids' => $assay_ids];
          $results = chado_query($sql, $args);
          while ($b = $results->fetchObject()) {
            $sname = $mapping[$b->assay_id];
            $this->samples[$sname]['assay_biomaterial_id'] = $b->assay_biomaterial_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $assay_ids = [];
        $mapping = [];
      }
    }
  }
  
  /**
   * Check if the features exist in the database.
   */
  private function findAcquisitions() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    $sql = "
      SELECT acquisition_id, assay_id
      FROM {acquisition}
      WHERE assay_id in (:assay_ids)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $assay_ids = [];
    $mapping = [];
    foreach ($this->samples as $name => $info) {
      $total++;
      $i++;
      
      $assay_ids[] = $info['assay_id'];
      $mapping[$info['assay_id']] = $name;
      
      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($assay_ids) > 0) {
          $args = [':assay_ids' => $assay_ids];
          $results = chado_query($sql, $args);
          while ($b = $results->fetchObject()) {
            $sname = $mapping[$b->assay_id];
            $this->samples[$sname]['acquisition_id'] = $b->acquisition_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $assay_ids = [];
        $mapping = [];
      }
    }
  }
  
  /**
   * Check if the features exist in the database.
   */
  private function findQuantifications() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    $sql = "
      SELECT quantification_id, acquisition_id
      FROM {quantification}
      WHERE acquisition_id in (:acquisition_ids)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $acquisition_ids = [];
    $mapping = [];
    foreach ($this->samples as $name => $info) {
      $total++;
      $i++;
      
      $acquisition_ids[] = $info['acquisition_id'];
      $mapping[$info['acquisition_id']] = $name;
      
      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($acquisition_ids) > 0) {
          $args = [':acquisition_ids' => $acquisition_ids];
          $results = chado_query($sql, $args);
          while ($b = $results->fetchObject()) {
            $sname = $mapping[$b->acquisition_id];
            $this->samples[$sname]['quantification_id'] = $b->quantification_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $acquisition_ids = [];
        $mapping = [];
      }
    }
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
          $mapping[$name] = $name;
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
   * 
   */
  private function verifyFeatures() {
    foreach ($this->features as $name => $feature) {
      if (!array_key_exists('feature_id', $feature)) {
        throw new Exception(t('Feature, !feature, does not exist in the database.',
            ['!feature' => $name]));
      }
    }
  }
  
  /**
   * Check if the features exist in the database.
   */
  private function findElements() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;
    $fname = $this->seqname;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $sql = "
      SELECT element_id, feature_id
      FROM {element}
      WHERE feature_id in (:feature_ids) and arraydesign_id = :arraydesign_id
    ";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $feature_ids = [];
    $mapping = [];
    foreach ($this->features as $name => $info) { 
      $feature_ids[] = $info['feature_id'];  
      $mapping[$info['feature_id']] = $name;
      $total++;
      $i++;
      
      // If we've reached the size of the batch then let's do the select.
      if ($i == $batch_size or $total == $num_features) {
        if (count($feature_ids) > 0) {
          $args = [
            ':feature_ids' => $feature_ids,
            ':arraydesign_id' => $this->arraydesign->getID(),
          ];
          $results = chado_query($sql, $args);
          while ($f = $results->fetchObject()) {
            $fname = $mapping[$f->feature_id];
            $this->features[$fname]['element_id'] = $f->element_id;
          }
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $feature_ids = [];
        $mapping = [];
      }
    }
  }
  
  /**
   * 
   */
  private function insertArrayDesign() {
    // Get objects for insert various items
    $manufacturer = new ChadoRecord('contact');
    $manufacturer->setValues([
      'name' => 'Not provided',
    ]);
    if ($manufacturer->find() == 0) {
      $manufacturer->setValue('description', 'Caution: This is a generic contact created by the expression module. This contact may be a dependency for an expression analysis. Delete with caution.');
      $manufacturer->insert();
    }
    
    $platform_type = new ChadoRecord('cvterm');
    $platform_type->setValues([
      'name' => 'null',
      'cv_id' => 1,
      'dbxref_id' => 1
    ]);
    if ($platform_type->find() == 0) {
      $platform_type->setValue('definition', 'NULL');
      $platform_type->insert();
    }
    
    $arraydesign = new ChadoRecord('arraydesign');
    $arraydesign->setValues([
      'name' => 'Not provided',
    ]);
    if($arraydesign->find() == 0) {
      $arraydesign->setValues([
        'manufacturer_id' => $manufacturer->getID(),
        'platformtype_id' => $platform_type->getID(),
        'name' => 'Not provided',
        'description' => 'Caution: This is a generic arraydesign created by the expression module. This arraydesign may be a dependency for an expression analysis. Delete with caution.',
      ]);
      $arraydesign->insert();
    }
    $this->arraydesign = $arraydesign;
  } 
  
  /**
   * Inserts the element records
   */
  private function insertElements() {
    $batch_size = 1000;
    $num_features = count(array_keys($this->features));
    $num_batches = (int) ($num_features / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $init_sql = "
      INSERT INTO {element}
        (feature_id, arraydesign_id)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->features as $feature) {  
      $skip = FALSE;
      
      $total++;
      if (array_key_exists('element_id', $feature)) {
        $skip = TRUE;
      }
      $i++;
      
      if (!$skip) {
        $sql .= "(:feature_id_$i, :arraydesign_id_$i),\n";
        $args[":feature_id_$i"] = $feature['feature_id'];
        $args[":arraydesign_id_$i"] = $this->arraydesign->getID();
      }
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_features) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }
  
  /**
   * Checks to see if the value should be inserted
   */
  private function checkValue($value) {
    // Don't insert missing values.
    if ($value == $this->missing_value) {
      return FALSE;
    }
    if ($value == '') {
      return FALSE;
    }
    if (!is_numeric($value)) {
      throw new Exception(t('Value in the expression data is not numeric: !value', ['!value' => $value]));
    }
    return TRUE;
  }
  
  /**
   * Retrieves an array of values for each feature.
   */
  private function getValues($feature_name) {    
    
    $feature = $this->features[$feature_name];
    $values = [];
    $fname = '';
    
    // Get the values from the matrix file.
    if ($this->filetype == 'mat') {
      $findex = $feature['findex'];
      $retval = fseek($this->files[0], $findex);
      if ($retval == -1) {
        throw new Exception(t('Cannot seek to file location, !findex, in matrix file.',
            ['!findex' => $findex]));
      }
      
      $line = fgets($this->files[0]);
      $col_values = explode($this->delim, trim($line));  
      $fname = array_shift($col_values);
      $values = $col_values;
      
      // Sanity check. Make sure the names match.
      if ($feature_name != $fname) {
        throw new Exception(t('Position in file, !file, for feature, !feature is incorrect.',
            ['!file' => $this->files_pathinfo[0]['basename'], '!feature' => $fname]));
      }
     
    }
    // Get the values from the column files
    else {
      
      // Get the row of data from the correct column file..
      foreach ($feature['findex'] as $fh_index => $findex) {
        $retval = fseek($this->files[$fh_index], $findex);
        if ($retval == -1) {
          throw new Exception(t('Cannot seek to file location, !findex, in file, !file.',
              ['!findex' => $findex, '!file' => $this->files_pathinfo[$fh_index]]));
        }
        $line = fgets($this->files[$fh_index]);
        $col_values = explode($this->delim, trim($line));
        $fname = $col_values[0];
        $values[$fh_index] = $col_values[1];
        
        // Sanity check. Make sure the names match.
        if ($feature_name != $fname) {
          throw new Exception(t('Position in file, !file, for feature, !feature is incorrect.',
              ['!file' => $this->files_pathinfo[0]['basename'], '!feature' => $fname]));
        }
      }
    }
    
    return $values;
  }
  
  /**
   * Inserts the element records
   */
  private function insertElementResults() {
    $batch_size = 1000;
    $num_values = count(array_keys($this->features)) * count($this->sample_order);
    $num_batches = (int) ($num_values / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $init_sql = "
      INSERT INTO {elementresult}
        (element_id, quantification_id, signal)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    $value = NULL;
    foreach ($this->features as $name => $feature) {  
      
      // Now iterate through the values and add the items to be inserted.
      $values = $this->getValues($name);
      for ($v = 0; $v < count($this->sample_order); $v++) {
        $total++;
        $i++;    
        
        // The feature may be missing in some column-based input files
        // and if so, won't have an entry in the values array.
        if (array_key_exists($v, $values)) {
          
          // Get the sample name and the value for this sample.
          $sample_name = $this->sample_order[$v];          
          $value = $values[$v];
                    
          // Check to see if the value is okay for inserting (i.e. is not missing).
          if ($this->checkValue($value)) {
            $sql .= "(:element_id_$i, :quantification_id_$i, :signal_$i),\n";
            $args[":element_id_$i"] = $feature['element_id'];
            $args[":quantification_id_$i"] = $this->samples[$sample_name]['quantification_id'];
            $args[":signal_$i"] = $value;
          }
        }
               
        // If we've reached the size of the batch then let's do the insert.
        if ($i == $batch_size or $total == $num_values) {
          if (count($args) > 0) {
            $sql = rtrim($sql, ",\n");
            $sql = $init_sql . $sql;
            chado_query($sql, $args);
          }
          $this->setItemsHandled($batch_num);
          $batch_num++;
          
          // Now reset all of the varables for the next batch.
          $sql = '';
          $i = 0;
          $args = [];
        }
      }
    }
  }
  
  /**
   * Inserts the node records.
   */
  private function insertBioMaterial() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $init_sql = "
      INSERT INTO {biomaterial}
        (taxon_id, name)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->samples as $name => $info) {
      $biomaterial_name = (array_key_exists('biomaterial_name', $info)) ? $info['biomaterial_name'] : $name;
      $skip = FALSE;
      $total++;
      $i++;
      
      // Skip samples that are already have a biomaterial.
      if ($info['biomaterial_id']) {
        $skip = TRUE;
      }          
      
      if (!$skip) {
        $sql .= "(:taxon_id_$i, :name_$i),\n";
        $args[":taxon_id_$i"] = $this->organism->getID();
        $args[":name_$i"] = $biomaterial_name;
      }
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }
  
  /** 
   * Generates an Assay name.
   */
  private function getAssayName($sample) {
    if ($sample['name'] != $sample['biomaterial_name']) {
      return 'Expression assay from, ' . $sample['name'] . ', from biological sample, ' . $sample['biomaterial_name']  . ', derived from the analysis named "' .  $this->analysis->getValue('name') . '"';
    }
    else {
      return 'Expression assay from, ' . $sample['name'] . ', derived from analysis "' .  $sample->analysis->getValue('name') . '"';
    }
  }
  
  /**
   * For each biomaterial add an assay record
   */
  private function insertAssays() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    // Get objects for insert various items
    $operator = new ChadoRecord('contact');
    $operator->setValues([
      'name' => 'Not provided',
    ]);    
    if ($operator->find() == 0) {
      $operator->setValue('description', 'Caution: This is a generic contact created by the expression module. This contact may be a dependency for an expression analysis. Delete with caution.');
      $operator->insert();
    }
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $init_sql = "
      INSERT INTO {assay}
        (arraydesign_id, operator_id, name)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->samples as $info) {
      $total++;
      $i++;
      $skip = FALSE;
      
      // Don't add assays' that already exist.
      if (array_key_exists('assay_id', $info)) {
        $skip = TRUE;
      }
      
      if (!$skip) {
        $sql .= "(:arraydesign_id_$i, :operator_id_$i, :name_$i),\n";
        $args[":arraydesign_id_$i"] = $this->arraydesign->getID();
        $args[":operator_id_$i"] = $operator->getID();
        $args[":name_$i"] = $this->getAssayName($info);
      }
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }
  /**
   * For each biomaterial add an assay record
   */
  private function insertAssayBiomaterials() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    // Get objects for insert various items
    $channel = new ChadoRecord('channel');
    $channel->setValues([
      'name' => 'Not provided',
    ]);
    if ($channel->find() == 0) {
      $channel->setValue('definition', 'Caution: This is a generic channel created by the expression module. This channel may be a dependency for an expression analysis. Delete with caution.');
      $channel->insert();
    }
    
    // Get objects for insert various items
    $operator = new ChadoRecord('contact');
    $operator->setValues([
      'name' => 'Not provided',
    ]);
    if ($operator->find() == 0) {
      $operator->setValue('description', 'Caution: This is a generic contact created by the expression module. This contact may be a dependency for an expression analysis. Delete with caution.');
      $operator->insert();
    }
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $init_sql = "
      INSERT INTO {assay_biomaterial}
        (assay_id, biomaterial_id, channel_id, rank)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->samples as $info) {
      $total++;
      $i++;      
      $skip = FALSE;
      
      // Don't add assay biomaterials if they already exist.
      if (array_key_exists('assay_biomaterial_id', $info)) {
        $skip = TRUE;
      }
      
      if (!$skip) {
        $sql .= "(:assay_id_$i, :biomaterial_id_$i, :channel_id_$i, 1),\n";
        $args[":assay_id_$i"] = $info['assay_id'];
        $args[":biomaterial_id_$i"] = $info['biomaterial_id'];
        $args[":channel_id_$i"] = $channel->getID();
      }
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }
  
  /**
   * 
   */
  private function deleteAcquisitions() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $sql = "DELETE from {acquisition} WHERE assay_id IN (:assay_ids)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $assay_ids = [];
    foreach ($this->samples as $info) {
      $assay_ids[] = $info['assay_id'];
      $total++;
      $i++;
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($assay_ids) > 0) {
          $args = [':assay_ids' => $assay_ids];
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $assay_ids = [];
      }
    }
  }
  /**
   *
   */
  private function deleteQuantificationProps() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $sql = "DELETE from {quantificationprop} WHERE quantification_id IN (:quantification_ids)";
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $quantification_ids = [];
    foreach ($this->samples as $info) {
      $quantification_ids[] = $info['quantification_id'];
      $total++;
      $i++;
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($quantification_ids) > 0) {
          $args = [':quantification_ids' => $quantification_ids];
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $i = 0;
        $quantification_ids = [];
      }
    }
  }
  
  /**
   * Generates an Assay name.
   */
  private function getAcquisitionName($sample) {
    if ($sample['name'] != $sample['biomaterial_name']) {
      return 'Measured expression from, ' . $sample['name'] . ', from biological sample, ' . $sample['biomaterial_name']  . ', derived from the analysis named "' .  $this->analysis->getValue('name') . '"';
    }
    else {
      return 'Measured expression from, ' . $sample['name'] . ', derived from analysis "' .  $sample->analysis->getValue('name') . '"';
    }
  }
  
  /**
   * For each biomaterial add an acquisition record
   */
  private function insertAcquisitions() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $init_sql = "
      INSERT INTO {acquisition}
        (assay_id, name)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->samples as $name => $info) {
      $total++;
      $i++;
      
      $sql .= "(:assay_id_$i, :name_$i),\n";
      $args[":assay_id_$i"] = $info['assay_id'];
      $args[":name_$i"] = $this->getAcquisitionName($info);
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }
  
  /**
   * For each biomaterial add an acquisition record
   */
  private function insertQuantifications() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $init_sql = "
      INSERT INTO {quantification}
        (acquisition_id, analysis_id, name)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->samples as $info) {
      $total++;
      $i++;
      
      $sql .= "(:acquisition_id_$i, :analysis_id_$i, :name_$i),\n";
      $args[":acquisition_id_$i"] = $info['acquisition_id'];
      $args[":analysis_id_$i"] = $this->analysis->getID();
      $args[":name_$i"] = $info['biomaterial_name'] . ' from ' . $this->analysis->getValue('name');
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }
  
  /**
   * For each biomaterial add an acquisition record
   */
  private function insertQuantificationProps() {
    $batch_size = 1000;
    $num_samples = count(array_keys($this->samples));
    $num_batches = (int) ($num_samples / $batch_size) + 1;
    
    $this->setItemsHandled(0);
    $this->setTotalItems($num_batches);
    
    $type_id = tripal_get_cvterm([
      'name' => 'unit_of_measure',
      'cv_id' => [
        'name' => 'sep',
      ],
    ])->cvterm_id;
    
    $init_sql = "
      INSERT INTO {quantificationprop}
        (quantification_id, type_id, value)
      VALUES
    ";
    
    $i = 0;
    $total = 0;
    $batch_num = 1;
    $sql = '';
    $args = [];
    foreach ($this->samples as $info) {
      $total++;
      $i++;
      
      $sql .= "(:quantification_id_$i, :type_id_$i, :value_$i),\n";
      $args[":quantification_id_$i"] = $info['quantification_id'];
      $args[":type_id_$i"] = $type_id;
      $args[":value_$i"] = $this->quantificationunits;
      
      // If we've reached the size of the batch then let's do the insert.
      if ($i == $batch_size or $total == $num_samples) {
        if (count($args) > 0) {
          $sql = rtrim($sql, ",\n");
          $sql = $init_sql . $sql;
          chado_query($sql, $args);
        }
        $this->setItemsHandled($batch_num);
        $batch_num++;
        
        // Now reset all of the varables for the next batch.
        $sql = '';
        $i = 0;
        $args = [];
      }
    }
  }
  
  
  /**
   * @see TripalImporter::run()
   */
  public function run() {

    $arguments = $this->arguments['run_args'];
    $this->missing_value = $arguments['missing_value'];
    $this->seqtype = $arguments['seqtype'];
    $this->seqname = $arguments['feature_uniquenames'];
    $this->arraydesign_id = $arguments['arraydesign_id']; 
    $this->quantificationunits = $arguments['quantificationunits'];     
    $this->filetype = $arguments['filetype'];
    $this->delim = $arguments['delimiter'] == 'tab' ? "\t" : ',';
    $this->create_biomaterials = $arguments['create_biomaterials'];
    $this->arraydesign_id = $arguments['arraydesign_id'];
    
    try {
      // Get the organism object.
      $this->organism = new ChadoRecord('organism');
      $this->organism->setValues(['organism_id' => $arguments['organism_id']]);
      $num_found = $this->organism->find();
      if ($num_found == 0) {
        throw new Exception(t("Cannot find the specified organism."));
      }
      
      // Get the analysis object.
      $this->analysis = new ChadoRecord('analysis');
      $this->analysis->setValues(['analysis_id' => $arguments['analysis_id']]);
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
      
      // Make sure an arary design record exists. If the user didn't
      // specify one then we'll make a generic one.
      if (!$this->arraydesign_id) {
        $this->insertArrayDesign();
      }
      else {
        $arraydesign = new ChadoRecord('arraydesign');
        $arraydesign->setValues([
          'arraydesign_id' => $this->arraydesign_id,
        ]);
        $this->arraydesign = $arraydesign;        
      }
      
      $this->logMessage("Step  1 of 23: Parsing input file...                                       ");
      $this->openFiles();
      $this->parseFiles();
      $this->logMessage("  Found " . count(array_keys($this->features)) . " features and " . count(array_keys($this->samples)) . " samples.");
      
      $this->logMessage("Step  2 of 23: Find Existing Features...                                   ");
      $this->findFeatures();
      $this->verifyFeatures();

      $this->logMessage("Step  3 of 23: Find Existing Elements...                                   ");
      $this->findElements();
      
      $this->logMessage("Step  4 of 23: Insert New Elements...                                      ");
      $this->insertElements();    

      $this->logMessage("Step  5 of 23: Get Newly Inserted Element IDs...                           ");
      $this->findElements();
            
      $this->logMessage("Step  6 of 23: Find NCBI BioSamples...                                     ");
      $this->logMessage("  Note: This step may take a while. It queries NCBI if any samples use GEO "); 
      $this->logMessage("        or SRA IDs.");
      $this->findNCBISamples();
     
      $this->logMessage("Step  7 of 23: Find Local BioSamples...                                    ");
      $this->findLocalSamples();

      $this->logMessage("Step  8 of 23: Insert New BioSamples...                                    ");
      if ($this->create_biomaterials) {
        $this->insertBioMaterial();
      }
      else {
        $this->logMessage("  Skipping: submitter indicated to not create BioSamples.                ");
      }
      
      $this->logMessage("Step  9 of 23: Get Newly Inserted BioSample IDs...                         ");
      if ($this->create_biomaterials) {
        $this->findLocalSamples();
      }
      else {
        $this->logMessage("  Skipping: submitter indicated to not create BioSamples.                ");
      }      
      
      $this->logMessage("Step 10 of 23: Find Existing Assays for Each BioSample...                  ");
      $this->findAssays();
      
      $this->logMessage("Step 11 of 23: Insert New Assays for BioSamples...                         ");
      $this->insertAssays();
      
      $this->logMessage("Step 12 of 23: Get Newly Inserted Assays IDs...                            ");
      $this->findAssays();

      $this->logMessage("Step 13 of 23: Find Assay Biomaterial Links...                             ");
      $this->findAssayBiomaterials();
      
      $this->logMessage("Step 14 of 23: Insert New Assay Biomaterial Links...                       ");
      $this->insertAssayBiomaterials();
      
      $this->logMessage("Step 15 of 23: Get Newly Inserted Assay Biomaterial Links...               ");
      $this->findAssayBiomaterials();

      $this->logMessage("Step 16 of 23: Clear Acquisition Data...                                   ");
      $this->deleteAcquisitions();
      
      $this->logMessage("Step 17 of 23: Insert Acquisition Data...                                  ");
      $this->insertAcquisitions();
      
      $this->logMessage("Step 18 of 23: Get Newly Inserted Acquisition Data...                      ");
      $this->findAcquisitions();
      
      $this->logMessage("Step 19 of 23: Insert Quantification Records...                            ");
      $this->insertQuantifications();
      
      $this->logMessage("Step 20 of 23: Get Newly Inserted Quantification Records...                ");
      $this->findQuantifications();
      
      $this->logMessage("Step 22 of 23: Clear Quantification Data for this Assay...                 ");
      $this->deleteQuantificationProps();
      
      $this->logMessage("Step 22 of 23: Insert Quantification Props...                              ");
      if ($this->quantificationunits) {
        $this->insertQuantificationProps();
      }
      else {
        $this->logMessage("  Skipping: no quantifcation unit specified.                             ");
      }
      
      $this->logMessage("Step 23 of 23: Insert Expression Data...                                   ");
      $this->insertElementResults();
    }
    catch (Exception $e) {
      $this->closeFiles();
      throw $e;
    }
    $this->closeFiles();
  }
}
