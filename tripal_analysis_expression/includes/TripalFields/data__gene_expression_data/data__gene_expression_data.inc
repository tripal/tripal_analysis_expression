<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class data__gene_expression_data extends ChadoField{

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Expression';

  // The default description for this field.
  public static $default_description = 'Visualize expression data';

  // The default widget for this field.
  public static $default_widget = 'data__gene_expression_data_widget';

  // The default formatter for this field.
  public static $default_formatter = 'data__gene_expression_data_formatter';

  // The module that manages this field.
  public static $module = 'tripal_analysis_expression';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_chado_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'searchable_keys' => [],
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'data',
    // The name of the term.
    'term_name' => 'gene_expression_data',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '2603',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'feature',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'feature_id',
    // The base table.
    'base_table' => 'feature',
  ];

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;
  
  private function loadAnalysis(&$entity) {
    
    $field = get_class();
    $analysis_id = $entity->chado_record_id;
    $features = $this->get_analysis_info($analysis_id);
    
    $sql = 'SELECT a.value FROM {chado.analysisprop} a
                WHERE a.analysis_id = :analysis_id
                AND a.type_id = :type_id';
    
    $cvterm_id = tripal_get_cvterm([
      'name' => 'data_source_file',
      'cv_id' => [
        'name' => 'local',
      ],
    ]) ?? new stdClass();
    $cvterm_id = $cvterm_id->cvterm_id ?? 1;
    
    $file = db_query($sql, [
      ':analysis_id' => $analysis_id,
      ':type_id' => $cvterm_id,
    ])->fetchField();
    
    $variables = ["features" => $features];
    $variables['file'] = $file;
    
    if (count($features["features"]) == 0) {
      $entity->{$field}['und'][0]["values"] = NULL;
      return;
    }
    
    // Save data into the entity
    $entity->{$field}['und'][0] = array_merge($entity->{$field}['und'][0], $variables);
  }
  
  /**
   * 
   */
  private function loadFeature(&$entity) {
    $record = $entity->chado_record;
    $settings = $this->instance['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    
    // Get the term used for the URL
    $url_type = tripal_get_chado_semweb_term('db', 'url');
    $feature_id = $record->feature_id;
    
    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0]['value'] = [];
    $entity->{$field_name}['und'][0]['value'][$url_type] = url('bio_data/' . $feature_id);
    return;
    
    // ChadoFields automatically load the chado column specified in the
    // default settings above. If that is all you need then you don't even
    // need to implement this function. However, if you need to add any
    // additional data to be used in the display, you should add it here.
    $field = get_class();
    $variables = $entity->{$field}['und'][0];
    $feature_id = isset($variables['chado-feature__feature_id']) ? $variables['chado-feature__feature_id'] : NULL;
    
    if ($feature_id === NULL) {
      return;
    }
    
    $hide_expression = variable_get('chado_feature_expression_hide_expression');
    $hide_biomaterial_labels = variable_get('chado_feature_expression_hide_biomaterial_labels');
    $limit_label_length = trim(variable_get('chado_feature_expression_limit_label_length'));
    $expression_display = variable_get('chado_feature_expression_expression_display');
    $biomaterial_display_width = variable_get('chado_feature_expression_biomaterial_display_width');
    $min_num_biomaterials = variable_get('chado_feature_expression_min_num_biomaterials');
    
    $has_exp = FALSE;
    
    $sql = "SELECT ER.signal, B.name, B.biomaterial_id, B.description, Q.quantification_id, AN.analysis_id, AN.name AS analysis_name
              FROM {elementresult} ER
              INNER JOIN {element} E ON E.element_id = ER.element_id
              INNER JOIN {feature} F ON F.feature_id = E.feature_id
              INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
              INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
              INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
              INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
              INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
              INNER JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
              WHERE F.feature_id = :feature_id
              ORDER BY AN.analysis_id ";
    $expressions = chado_query($sql, [":feature_id" => $feature_id]);
    
    $variables['analyses'] = $this->get_analyses($feature_id);
    
    $last_signal = NULL;
    $last_analysis_id = NULL;
    $array_to_encode = [];
    foreach ($expressions as $exp) {
      //if this analysis isn't in the array yet, initialize it
      if (!isset($array_to_encode[$exp->analysis_id])) {
        $array_to_encode[$exp->analysis_id] = [
          "analysis_id" => $exp->analysis_id,
          "analysis_name" => $exp->analysis_name,
          "biomaterials" => [],
        ];
      }
      $has_exp = TRUE;
      
      //Get units in quantificationprop
      $unit_prop = chado_get_property([
        'table' => 'quantification',
        'id' => $exp->quantification_id,
      ], ['type_name' => 'unit_of_measure', 'cv_name' => 'sep']);
      $unit_prop ? $units = $unit_prop->value : $units = '';
      
      //Get all properties for biosample/biomaterial
      $values = [
        'biomaterial_id' => $exp->biomaterial_id,
      ];
      $biomat_object = chado_generate_var('biomaterial', $values);
      $result = chado_expand_var($biomat_object, 'table', 'biomaterialprop');
      $properties = $result->biomaterialprop;
      $propstore = [];
      
      if (is_object($properties)) {
        $properties = [];
      }
      
      if (!$properties) {
        $properties = [];
      }
      
      foreach ($properties as $prop) {
        //get the prop name and value
        // $property_cvterm_id = $prop->type_id->cvterm_id;
        // $property_cv_id = $prop->type_id->cv_id;
        if ((isset($prop->value))) { //for some reason we have stuff coming back that is not properties.  this will filter them.
          $property_name = $prop->type_id->name;
          $property_value = $prop->value;
          $propstore[$property_name] = $property_value;
        }
      }
      
      $node_present = chado_get_record_entity_by_table('biomaterial',
          $exp->biomaterial_id);
      
      //Add a property for no sorting ie by biosample name
      $propstore["Biosample Name"] = $exp->name;
      
      //Build entry for this biosample/biomaterial, append it to the existing array, and store it in the master array
      $biosample_properties = [
        "name" => $exp->name,
        "intensity" => $exp->signal,
        "units" => $units,
        "description" => $exp->description,
        "properties" => $propstore,
        "node" => $node_present,
      ];
      $existing_samples = $array_to_encode[$exp->analysis_id]["biomaterials"];
      array_push($existing_samples, $biosample_properties);
      $array_to_encode[$exp->analysis_id]["biomaterials"] = $existing_samples;
    }
    $json_exp = json_encode($array_to_encode);
    if ($expressions->rowCount() < $min_num_biomaterials) {
      $json_exp = "";
    }
    
    if ($hide_expression == 1) {
      $json_exp = "";
    }
    
    $variables['has_exp'] = $has_exp;
    $variables['json_exp'] = $json_exp;
    $variables['limit_label_length'] = $limit_label_length;
    $variables['hide_expression'] = $hide_expression;
    $variables['hide_biomaterial_labels'] = $hide_biomaterial_labels;
    $variables['expression_display'] = $expression_display;
    $variables['biomaterial_display_width'] = $biomaterial_display_width;
    $variables['min_num_biomaterials'] = $min_num_biomaterials;
    
    // Save data into the entity
    $entity->{$field}['und'][0] = array_merge($entity->{$field}['und'][0], $variables);
    
  }

  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   */
  public function load($entity) {
    if ($entity->chado_table === "analysis") {
      $this->loadAnalysis($entity);
    }
    if ($entity->chado_table === "feature") {
      $this->loadFeature($entity);
    }
    return $entity;
  }

  /**
   * Get expression analyses for a certain feature.
   *
   * @param $feature_id
   *
   * @return bool|\DatabaseStatementInterface|null
   */
  protected function get_analyses($feature_id) {
    $sql = "SELECT DISTINCT(AN.analysis_id), AN.name
              FROM {elementresult} ER
              INNER JOIN {element} E ON E.element_id = ER.element_id
              INNER JOIN {feature} F ON F.feature_id = E.feature_id
              INNER JOIN {quantification} Q ON Q.quantification_id = ER.quantification_id
              INNER JOIN {acquisition} AQ ON AQ.acquisition_id = Q.acquisition_id
              INNER JOIN {assay} A ON A.assay_id = AQ.assay_id
              INNER JOIN {assay_biomaterial} AB ON AB.assay_id = A.assay_id
              INNER JOIN {biomaterial} B ON B.biomaterial_id = AB.biomaterial_id
              INNER JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
              WHERE F.feature_id = :feature_id";
    $analyses = chado_query($sql, [":feature_id" => $feature_id]);
    if ($analyses) {
      $analyses = $analyses->fetchAll();
      foreach ($analyses as $key => $analysis) {
        $analyses[$key]->url = 'bio_data/' . chado_get_record_entity_by_table('analysis',
            $analysis->analysis_id);
      }
      return $analyses;
    }
    else {
      return NULL;
    }
  }

  protected function get_analysis_info($analysis_id) {
    $sql = "SELECT DISTINCT(feature_id)
      from {expression_feature_all}
      WHERE analysis_id = :analysis_id
    ";

    $features = [];
    $biomaterials = '';

    $result = chado_query($sql, [":analysis_id" => $analysis_id]);
    if ($result) {
      $result = $result->fetchAll();

      foreach ($result as $feature) {
        $features[] = $feature->feature_id;
      }
    }

    $sql = "SELECT DISTINCT(biomaterial_name)
      from {expression_feature_all}
      WHERE analysis_id = :analysis_id
    ";
    $result = chado_query($sql, [":analysis_id" => $analysis_id]);
    if ($result) {
      $biomaterials = $result->fetchAll();
    }

    $output = ["features" => $features, "biomaterials" => $biomaterials];
    return ($output);
  }

}
