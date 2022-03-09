<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class sep__biological_sample extends ChadoField{

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Biomaterial Browser';

  // The default description for this field.
  public static $description = 'List of related biomaterials';

  // The default widget for this field.
  public static $default_widget = 'sep__biological_sample_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sep__biological_sample_formatter';

  // The module that manages this field.
  public static $module = 'tripal_biomaterial';

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
    // The DATABASE name, as it appears in chado.db. This also builds the
    // link-out url.  In most cases this will simply be the CV name. In
    // some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'sep',
    // The name of the term.
    'term_name' => 'biological sample',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '00195',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // The number of items to show on a page.
    'items_per_page' => 10,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed. Fields that are slow loading should have
    // auto_attach set to FALSE so that their values can be attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => 'biomaterial',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'biomaterial_id',
    // The base table.
    'base_table' => 'biomaterial',
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

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;

  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   * @see TripalField::elements()
   */
  public function elementInfo() {

    $field_term = $this->getFieldTermID();

    $name_term = chado_get_semweb_term('biomaterial', 'name');
    $description_term = chado_get_semweb_term('biomaterial', 'description');
    // Use fixed terms for these values which derive from the biomaterialprop table,
    // type_id + value columns. This module's installer loads biomaterial_property cv
    // and links it to tripal db. tripal_eutils module uses ncbi_properties cv linked
    // to ncbi_properties db. Static term here allows this field to work with both.
    $tissue_term = 'tripal:tissue';
    $treatment_term = 'tripal:treatment';

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'type' => 'xs:complexType',
        'readonly' => FALSE,
        'elements' => [
          $description_term => [
            'searchable' => FALSE,
            'label' => 'Description',
            'help' => 'A textual description of the biomaterial.',
            'operations' => [],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $name_term => [
            'searchable' => FALSE,
            'label' => 'Name',
            'help' => 'The name of the biomaterial.',
            'operations' => [],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => TRUE,
          ],
          $tissue_term => [
            'searchable' => FALSE,
            'label' => 'Tissue',
            'help' => 'The anatomical tissue type that the biomaterial represents.',
            'operations' => [],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $treatment_term => [
            'searchable' => FALSE,
            'label' => 'Treatment',
            'help' => 'The treatment that the biomaterial underwent prior to analysis.',
            'operations' => [],
            'sortable' => TRUE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          'entity' => [
            'searchable' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
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
   *
   * @return void
   * @throws \Exception
   */
  public function load($entity) {
    $record = $entity->chado_record;

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $name_term = chado_get_semweb_term('biomaterial', 'name');
    $description_term = chado_get_semweb_term('biomaterial', 'description');
    // Use fixed terms for these values which derive from the biomaterialprop table,
    // type_id + value columns. This module's installer loads biomaterial_property cv
    // and links it to tripal db. tripal_eutils module uses ncbi_properties cv linked
    // to ncbi_properties db. Static term here allows this field to work with both.
    $tissue_term = 'tripal:tissue';
    $treatment_term = 'tripal:treatment';

    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . 'biomaterial_id' => '',
      'chado-' . $field_table . '__' . 'tissue' => '',
      'chado-' . $field_table . '__' . 'treatment' => '',
      'chado-' . $field_table . '__' . 'description' => '',
    ];

    // Chado queries are quite different depending on our content type, use custom sql for each.
    // Specification of cv is intentionally left out for getting the properties from the
    // biomaterialprop table, since the term may exist in more than one cv, e.g. 'tissue'
    // for this module is in biomaterial_property cv, eutils module uses ncbi_properties cv
    if ($entity->chado_table == 'organism') {
      // Organism is linked directly using the taxon_id column in the biomaterial table.
      $sql = "SELECT DISTINCT B.biomaterial_id, B.name, B.description, "
           . "PROP1.value AS tissue, PROP2.value AS treatment "
           . "FROM {biomaterial} B "
           . "LEFT JOIN {biomaterialprop} PROP1 ON PROP1.biomaterial_id = B.biomaterial_id "
           .   "AND PROP1.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'tissue') "
           . "LEFT JOIN {biomaterialprop} PROP2 ON PROP2.biomaterial_id = B.biomaterial_id "
           .   "AND PROP2.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'treatment') "
           . "WHERE B.taxon_id=:record_id "
           . "ORDER BY B.name ASC";
    }
    elseif ($entity->chado_table == 'project') {
      // Project is linked through the biomaterial_project table.
      $sql = "SELECT DISTINCT B.biomaterial_id, B.name, B.description, "
           . "PROP1.value AS tissue, PROP2.value AS treatment "
           . "FROM {biomaterial_project} BP "
           . "LEFT JOIN {biomaterial} B ON B.biomaterial_id = BP.biomaterial_id "
           . "LEFT JOIN {biomaterialprop} PROP1 ON PROP1.biomaterial_id = B.biomaterial_id "
           .   "AND PROP1.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'tissue') "
           . "LEFT JOIN {biomaterialprop} PROP2 ON PROP2.biomaterial_id = B.biomaterial_id "
           .   "AND PROP2.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'treatment') "
           . "WHERE BP.project_id=:record_id "
           . "ORDER BY B.name ASC";
    }
    elseif ($entity->chado_table == 'analysis') {
      // Analysis can be linked using two possiple paths:
      // via assay_biomaterial+acquisition+quantification,
      // or via biomaterial_project+project_analysis.
      $sql = "SELECT DISTINCT * FROM ("
           . "SELECT DISTINCT B1.biomaterial_id, B1.name, B1.description, "
           . "PROP1.value AS tissue, PROP2.value AS treatment "
           . "FROM {biomaterial} B1 "
           . "JOIN {assay_biomaterial} AB ON B1.biomaterial_id = AB.biomaterial_id "
           . "JOIN {acquisition} AQ ON AQ.assay_id = AB.assay_id "
           . "JOIN {quantification} Q ON Q.acquisition_id = AQ.acquisition_id "
           . "LEFT JOIN {biomaterialprop} PROP1 ON PROP1.biomaterial_id = B1.biomaterial_id "
           .   "AND PROP1.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'tissue') "
           . "LEFT JOIN {biomaterialprop} PROP2 ON PROP2.biomaterial_id = B1.biomaterial_id "
           .   "AND PROP2.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'treatment') "
           . "WHERE Q.analysis_id=:record_id "
           . "UNION "
           . "SELECT DISTINCT B2.biomaterial_id, B2.name, B2.description, "
           . "PROP3.value AS tissue, PROP4.value AS treatment "
           . "FROM {biomaterial} B2 "
           . "JOIN {biomaterial_project} BP ON B2.biomaterial_id = BP.biomaterial_id "
           . "JOIN {project_analysis} PA ON PA.project_id = BP.project_id "
           . "LEFT JOIN {biomaterialprop} PROP3 ON PROP3.biomaterial_id = B2.biomaterial_id "
           .   "AND PROP3.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'tissue') "
           . "LEFT JOIN {biomaterialprop} PROP4 ON PROP4.biomaterial_id = B2.biomaterial_id "
           .   "AND PROP4.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'treatment') "
           . "WHERE PA.analysis_id=:record_id "
           . ") U ORDER BY name ASC";
    }
    elseif ($entity->chado_table == 'assay') {
      // Assay is linked through the assay_biomaterial table
      $sql = "SELECT DISTINCT B.biomaterial_id, B.name, B.description, "
           . "PROP1.value AS tissue, PROP2.value AS treatment "
           . "FROM {biomaterial} B "
           . "JOIN {assay_biomaterial} AB ON B.biomaterial_id = AB.biomaterial_id "
           . "LEFT JOIN {biomaterialprop} PROP1 ON PROP1.biomaterial_id = B.biomaterial_id "
           .   "AND PROP1.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'tissue') "
           . "LEFT JOIN {biomaterialprop} PROP2 ON PROP2.biomaterial_id = B.biomaterial_id "
           .   "AND PROP2.type_id IN (SELECT cvterm_id FROM {cvterm} WHERE name = 'treatment') "
           . "WHERE AB.assay_id=:record_id "
           . "ORDER BY B.name ASC";
    }
    else { // field is on an unsupported content type
      return;
    }

    $args = [':record_id' => $entity->chado_record_id];
    $records = chado_query($sql, $args);

    $i = 0;
    while ($record = $records->fetchObject()) {
      $entity->{$field_name}['und'][$i] = [
        'value' => [
          $name_term => $record->name,
          $description_term => $record->description,
          $tissue_term => $record->tissue,
          $treatment_term => $record->treatment,
        ],
        'chado-' . $field_table . '__' . $pkey => $entity->chado_record_id,
        'chado-' . $field_table . '__' . 'biomaterial_id' => $record->biomaterial_id,
        'chado-' . $field_table . '__' . 'description' => $record->description,
        'chado-' . $field_table . '__' . 'name' => $record->name,
        'chado-' . $field_table . '__' . 'tissue' => $record->tissue,
        'chado-' . $field_table . '__' . 'treatment' => $record->treatment,
      ];

      // Is there a published entity for this biomaterial?
      $entity_id = chado_get_record_entity_by_table('biomaterial', $record->biomaterial_id);
      if ($entity_id) {
        $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $entity_id;
      }

      $i++;
    }
//ob_start(); var_dump($entity); $astring = ob_get_clean(); //@@@
//file_put_contents( "/dev/shm/entity", "CP1 bio entity=\"$astring\"\n", FILE_APPEND ); //@@@
  }
}
