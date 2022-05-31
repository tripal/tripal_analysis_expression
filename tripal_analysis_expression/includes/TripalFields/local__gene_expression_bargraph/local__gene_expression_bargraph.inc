<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class local__gene_expression_bargraph extends ChadoField{

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Expression Bargraph';

  // The default description for this field.
  public static $default_description = 'Visualization of gene expression data using a bargraph';

  // The default widget for this field.
  public static $default_widget = 'local__gene_expression_bargraph_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__gene_expression_bargraph_formatter';

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
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'Gene Expression Bargraph',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'gene_expression_bargraph',
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
    $settings = $this->instance['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    
    // Get the term used for the URL
    $url_term = tripal_get_chado_semweb_term('db', 'url');
    
    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0]['value'] = [];
    $entity->{$field_name}['und'][0]['value'][$url_term] = url("bio_data/" . $entity->id, ['absolute' => TRUE]);
    return $entity;
  }
  
  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();    
    $url_term = tripal_get_chado_semweb_term('db', 'url');
    
    return [
      $field_term => [
        'operations' => [],
        'sortable' => FALSE,
        'searchable' => FALSE,
        'readonly' => FALSE,
        'type' => 'xs:complexType',
        'elements' => [
          $url_term => [
            'searchable' => FALSE,
            'name' => 'URL',
            'operations' => [],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => TRUE,
          ],            
        ],
      ],
    ];
  }
}
