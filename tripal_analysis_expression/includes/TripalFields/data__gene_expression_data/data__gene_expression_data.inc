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
  public static $default_label = 'Expression Data';

  // The default description for this field.
  public static $default_description = 'Expression data';

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
    

  /**
   * Loads the field values from the underlying data store.
   *
   */
  public function load($entity) {
    
    $record = $entity->chado_record;
    $chado_table = $entity->chado_table;
    $field_name = $this->field['field_name'];
    
    // Get the list of analyses
    $analyses = [];
    if ($chado_table === "analysis") {      
      $analyses[$record->analysis_id] = [
        'name' => $record->name,
        'entity_id' => $entity->id,
      ];
    }
    if ($chado_table === "feature") {
      $sql = "
        SELECT * 
        FROM (SELECT DISTINCT analysis_id
              FROM {expression_feature_all}
              WHERE feature_id = :feature_id) as TBL1
          INNER JOIN {analysis} A ON TBL1.analysis_id = A.analysis_id  
      ";      
      $results = chado_query($sql, [':feature_id' => $record->feature_id]);
      while ($analysis = $results->fetchObject()) {        
        // Only iclude analyses that are published.
        $entity_id = chado_get_record_entity_by_table('analysis', $analysis->analysis_id);
        if ($entity_id) {
          $analyses[$analysis->analysis_id] = [
            'entity_id' => $entity_id,
            'name' => $analysis->name,
          ];
        }
      }      
    }
    
    if (count(array_keys($analyses)) == 0) {
      $entity->{$field_name}['und'][0]['value'] = [];
      return $entity;
    }

    // Get the number of features.
    $values = [];
    $sql = "
      SELECT analysis_id, count(feature_id) as num_features
      FROM 
        (SELECT DISTINCT analysis_id, feature_id
         FROM {expression_feature_all} EFA  
         WHERE analysis_id IN (:analysis_ids)) AS TBL1
      GROUP BY analysis_id
    ";
    $results = chado_query($sql, [":analysis_ids" => array_keys($analyses)]);
    while ($result = $results->fetchObject()) {
      
      // Analysis information.
      $values[$result->analysis_id]['operation:2945'] = [
        'entity' => 'TripalEntity:' . $analyses[$result->analysis_id]['entity_id'],
        'schema:name' => $analyses[$result->analysis_id]['name']
      ];
      
      // Description statistics.
      $values[$result->analysis_id]['AFP:0003636'][] = [
        'NCIT:C25463' => $result->num_features,
        'NCIT:C25284' => 'Feature',
      ];
      
      // Data Download link
      if ($chado_table == 'analysis') {
        $values[$result->analysis_id]['schema:DataDownload'] = url("tripal/analysis-expression/download") . "?analysis_id=" . $result->analysis_id;          
      }
      if ($chado_table == 'feature') {
        $values[$result->analysis_id]['schema:DataDownload'] = url("tripal/analysis-expression/download") . "?feature_ids=" . $record->feature_id . "&analysis_id=" . $result->analysis_id;
      }
    }
      
    // Get the number of biomaterials.
    $sql = "
      SELECT analysis_id, count(biomaterial_name) as num_biomaterials
      FROM
        (SELECT DISTINCT analysis_id, biomaterial_name
         FROM {expression_feature_all}
         WHERE analysis_id IN (:analysis_ids)) AS TBL1
      GROUP BY analysis_id
    ";
    $results = chado_query($sql, [":analysis_ids" => array_keys($analyses)]);
    while ($result = $results->fetchObject()) {
      
      // Analysis information.
      $values[$result->analysis_id]['operation:2945'] = [
        'entity' => 'TripalEntity:' . $analyses[$result->analysis_id]['entity_id'],
        'schema:name' => $analyses[$result->analysis_id]['name']
      ];
      
      // Description statistics.
      $values[$result->analysis_id]['AFP:0003636'][] = [
        'NCIT:C25463' => $result->num_biomaterials,
        'NCIT:C25284' => 'Sample'
      ];
      
      // Data Download link
      if ($chado_table == 'analysis') {
        $values[$result->analysis_id]['schema:DataDownload'] = NULL;
      }
      if ($chado_table == 'feature') {
        $values[$result->analysis_id]['schema:DataDownload'] = url("tripal/analysis-expression/download",
            ['absolute' => TRUE, 'query' => ['analysis_id' => $result->analysis_id, 'feature_ids' => $record->feature_id]]);
      }
    }
       
    // Set some defaults for the empty record.
    $i = 0;
    foreach ($values as $value) {
      $entity->{$field_name}['und'][$i]['value'] = $value;
      $i++;
    }
    return $entity;
  }
  
  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    
    $analysis_term = 'operation:2945';
    $type_term = 'NCIT:C25463';
    $count_term = 'NCIT:C25463';
    $dlink_term = 'schema:DataDownload';
    $stats_term = 'AFP:0003636';
    $name_term = 'schema:name';
    
    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'readonly' => FALSE,
        'type' => 'xs:complexType',
        'elements' => [
          $analysis_term => [
            'searchable' => FALSE,
            'name' => 'Analysis',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => TRUE,
            'elements' => [
              'entity' => [
                'searchable' => FALSE,
              ],
              $name_term => [
                'searchable' => FALSE,
                'name' => 'Name',
                'operations' => [],
                'sortable' => FALSE,
                'readonly' => TRUE,
                'type' => 'xs:string',
                'required' => FALSE,
              ],
            ],
          ],
          $stats_term => [
            'searchable' => FALSE,
            'name' => 'Descriptive Statistics',
            'operations' => [],
            'sortable' => FALSE,
            'readonly' => TRUE,
            'type' => 'xs:string',
            'required' => FALSE,
            'elements' => [
              $count_term => [
                'searchable' => FALSE,
                'name' => 'Count',
                'operations' => [],
                'sortable' => FALSE,
                'readonly' => TRUE,
                'type' => 'xs:string',
                'required' => FALSE,
              ],
              $type_term => [
                'searchable' => FALSE,
                'name' => 'Element Type',
                'operations' => [],
                'sortable' => FALSE,
                'readonly' => TRUE,
                'type' => 'xs:string',
                'required' => FALSE,
              ]
            ],
          ],
          $dlink_term => [
            'searchable' => FALSE,
            'name' => 'Data Download',
            'operations' => [],
            'sortable' => FALSE,
            'readonly' => TRUE,
            'type' => 'xs:string',
            'required' => FALSE,
          ]
        ],
      ],
    ];
  }
}
