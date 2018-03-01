<?php

/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class sep__biological_sample extends ChadoField {

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
  public static $default_description = 'List of biomaterials related to an organism';

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
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'sep',
    // The name of the term.
    'term_name' => 'biological sample',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '00195',
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
   *
   *
   */
  public function load($entity) {
    if ($entity->chado_table == 'analysis') {
      return $this->load_for_analysis($entity);
    }

    if ($entity->chado_table == 'organism') {
      return $this->load_for_organism($entity);
    }
  }

  /**
   * Load data for analysis pages.
   *
   * @param $entity
   *
   * @return mixed
   */
  protected function load_for_analysis(&$entity) {
    $field = get_class();
    $entity->{$field}['und'][0] = NULL;//set default null
    $analysis_id = $entity->chado_record_id;

    $sql = 'SELECT B.biomaterial_id, B.name, B.description
              FROM {biomaterial} B
              JOIN {assay_biomaterial} AB ON B.biomaterial_id = AB.biomaterial_id
              JOIN {assay} A ON A.assay_id = AB.assay_id
              JOIN {acquisition} AQ ON AQ.assay_id = A.assay_id
              JOIN {quantification} Q ON Q.acquisition_id = AQ.acquisition_id
              JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
              WHERE AN.analysis_id=:analysis_id';

    $results = chado_query($sql, [":analysis_id" => $analysis_id])->fetchAll();

    if ($results) {
      $i = 0;
      foreach ($results as $biomaterial) {

        $b_record_id = $biomaterial->biomaterial_id;

        $values = [
          'biomaterial_id' => $b_record_id,
        ];

        $biomat_full = chado_generate_var("biomaterial", $values);
        $expanded = chado_expand_var($biomat_full, 'table', 'biomaterialprop');
        $properties = $expanded->biomaterialprop;

        $treatment = NULL;
        $tissue = NULL;

        foreach ($properties as $property) {
          $name = strtolower($property->type_id->name);
          if ($name == "tissue") {
            $tissue = $property->value;
          }
          if ($name == "treatment") {
            $treatment = $property->value;
          }
        }
        $biomaterial_entity_id = chado_get_record_entity_by_table("biomaterial", $b_record_id);

        $value = [
          'name' => $biomaterial->name,
          'record_id' => $b_record_id,
          'entity_id' => $biomaterial_entity_id,
          'description' => $biomaterial->description,
          'tissue' => $tissue,
          'treatment' => $treatment,
        ];

        $entity->{$field}['und'][$i]['value'] = $value;

        $i++;
      }
    }

    return $entity;
  }

  /**
   * Load data for organism pages.
   *
   * @param $entity
   *
   * @return mixed
   */
  protected function load_for_organism(&$entity) {
    $field = get_class();
    $entity->{$field}['und'][0] = NULL;
    $organism_id = $entity->chado_record_id;

    $sql = 'SELECT B.biomaterial_id, B.name, B.description
            FROM {biomaterial} B
            WHERE B.taxon_id = :organism_id';

    $results = chado_query($sql, [":organism_id" => $organism_id])->fetchAll();

    $i = 0;
    if ($results) {
      foreach ($results as $biomaterial) {
        $b_record_id = $biomaterial->biomaterial_id;

        $values = [
          'biomaterial_id' => $b_record_id,
        ];

        $biomat_full = chado_generate_var("biomaterial", $values);
        $expanded = chado_expand_var($biomat_full, 'table', 'biomaterialprop');
        $properties = $expanded->biomaterialprop;

        $treatment = NULL;
        $tissue = NULL;

        if ($properties) {

          foreach ($properties as $property) {
	if (property_exists($property, "type_id") && property_exists($property, "name"){


            $name = strtolower($property->type_id->name);
            if ($name == "tissue") {
              $tissue = $property->value;
            }
            if ($name == "treatment") {
              $treatment = $property->value;
            }
          }
        }
      }

        $biomaterial_entity_id = chado_get_record_entity_by_table("biomaterial", $b_record_id);
        $value = [
          'name' => $biomaterial->name,
          'record_id' => $b_record_id,
          'entity_id' => $biomaterial_entity_id,
          'description' => $biomaterial->description,
          'tissue' => $tissue,
          'treatment' => $treatment,
        ];

        $entity->{$field}['und'][$i]['value'] = $value;

        $i++;
      }
    }

    return $entity;
  }
}
