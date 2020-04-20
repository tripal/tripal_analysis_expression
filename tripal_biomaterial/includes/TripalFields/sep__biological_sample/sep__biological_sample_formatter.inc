<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class sep__biological_sample_formatter extends ChadoFieldFormatter{

  // The default label for this field.
  public static $default_label = 'Biomaterials Browser';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sep__biological_sample'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

  /**
   * Provides the field's setting form.
   *
   * This function corresponds to the hook_field_formatter_settings_form()
   * function of the Drupal Field API.
   *
   * The settings form appears on the 'Manage Display' page of the content
   * type administration page. This function provides the form that will
   * appear on that page.
   *
   * To add a validate function, please create a static function in the
   * implementing class, and indicate that this function should be used
   * in the form array that is returned by this function.
   *
   * This form will not be displayed if the formatter_settings_summary()
   * function does not return anything.
   *
   * param $field
   *   The field structure being configured.
   * param $instance
   *   The instance structure being configured.
   * param $view_mode
   *   The view mode being configured.
   * param $form
   *   The (entire) configuration form array, which will usually have no use
   *   here.  Typically for reference only.
   * param $form_state
   *   The form state of the (entire) configuration form.
   *
   * @return
   *   A Drupal Form array containing the settings form for this field.
   */
  public function settingsForm($view_mode, $form, &$form_state) {

  }

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
   *
   * @param $element
   * @param $entity_type
   * @param $entity
   * @param $langcode
   * @param $data_items
   * @param $display
   *
   * @return void
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $base_table = $entity->chado_table;

    if (empty($items) || !($count = $items[0]['value'])) {
      $element[] = ['#markup' => 'There are no linked BioSamples.'];
      return;
    }

    $per_page = 10;
    $page = pager_default_initialize($count, $per_page);

    $headers = [
      'Name',
      'Tissue',
      'Treatment',
      'Description',
    ];

    $rows = [];

    if ($entity->chado_table === 'analysis') {
      $data_items = $this->loadAnalysis($entity, $page * $per_page, $per_page);
    }
    elseif ($entity->chado_table === 'organism') {
      $data_items = $this->loadOrganism($entity, $page * $per_page, $per_page);
    }
    else {
      return;
    }

    foreach ($data_items as $item) {

      $b_contact = NULL;
      $tissue = NULL;
      $treatment = NULL;
      $description = NULL;

      $biomaterial = $item;
      if (empty($biomaterial)) {
        return NULL;
      }

      $b_name = $biomaterial["name"];
      $id = $biomaterial["entity_id"];

      if ($id) {
        $b_name = l($b_name, 'bio_data/' . $id);
      }

      $tissue = $biomaterial['tissue'];
      $treatment = $biomaterial['treatment'];

      $description = $biomaterial["description"];
      if (strlen($description) > 300) {
        $description = substr($description, 0, 300);
        $description .= "...";
      }

      $row = [
        $b_name,
        // $b_contact ?: 'Not set',
        $tissue ?: 'Not set',
        $treatment ?: "Not set",
        $description,
      ];

      $rows[] = $row;
    }

    $output = theme('table', [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => ['class' => ['table-layout-fixed']],
    ]);

    $pager = theme('pager', ['quantity', $count]);
    $output .= $this->ajaxifyPager($pager, $entity);

    $element[] = ['#markup' => $output];
  }

  /**
   * Load data for analysis pages.
   *
   * @param $entity
   *
   * @return mixed
   */
  protected function loadAnalysis(&$entity, $offset, $limit) {
    $field = get_class();
    $entity->{$field}['und'] = [];
    $analysis_id = $entity->chado_record_id;

    $sql = 'SELECT B.biomaterial_id, B.name, B.description
              FROM {biomaterial} B
              JOIN {assay_biomaterial} AB ON B.biomaterial_id = AB.biomaterial_id
              JOIN {assay} A ON A.assay_id = AB.assay_id
              JOIN {acquisition} AQ ON AQ.assay_id = A.assay_id
              JOIN {quantification} Q ON Q.acquisition_id = AQ.acquisition_id
              JOIN {analysis} AN ON AN.analysis_id = Q.analysis_id
              WHERE AN.analysis_id=:analysis_id
              ORDER BY B.name ASC
              OFFSET :offset
              LIMIT :limit';

    $results = chado_query($sql, [
      ":analysis_id" => $analysis_id,
      ':offset' => $offset,
      ':limit' => $limit,
    ])->fetchAll();

    if (!$results) {
      return [];
    }

    $rows = [];
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

        if (!empty($properties)) {
          foreach ($properties as $property) {
            $name = strtolower($property->type_id->name);
            if ($name == "tissue") {
              $tissue = $property->value;
            }
            if ($name == "treatment") {
              $treatment = $property->value;
            }
          }
        }

        $biomaterial_entity_id = chado_get_record_entity_by_table("biomaterial",
          $b_record_id);

        $value = [
          'name' => $biomaterial->name,
          'record_id' => $b_record_id,
          'entity_id' => $biomaterial_entity_id,
          'description' => $biomaterial->description,
          'tissue' => $tissue,
          'treatment' => $treatment,
        ];

        $rows[] = $value;

        $i++;
      }
    }

    return $rows;
  }

  /**
   * Load data for organism pages.
   *
   * @param $entity
   * @param int $offset
   *
   * @return mixed
   * @throws \Exception
   */
  protected function loadOrganism($entity, $offset, $per_page) {
    $field = get_class();
    $entity->{$field}['und'] = [];
    $organism_id = $entity->chado_record_id;

    $sql = 'SELECT B.biomaterial_id, B.name, B.description
            FROM {biomaterial} B
            WHERE B.taxon_id = :organism_id
            ORDER BY B.name ASC
            OFFSET :offset
            LIMIT :limit';

    $results = chado_query($sql, [
      ":organism_id" => $organism_id,
      ":offset" => $offset,
      ':limit' => $per_page,
    ])->fetchAll();

    if (!$results) {
      return [];
    }

    $i = 0;
    $rows = [];
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
          if (property_exists($property,
              'type_id') && property_exists($property->type_id, 'name')) {
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

      $biomaterial_entity_id = chado_get_record_entity_by_table("biomaterial",
        $b_record_id);
      $value = [
        'name' => $biomaterial->name,
        'record_id' => $b_record_id,
        'entity_id' => $biomaterial_entity_id,
        'description' => $biomaterial->description,
        'tissue' => $tissue,
        'treatment' => $treatment,
      ];

      $rows[] = $value;

      $i++;
    }

    return $rows;
  }

  /**
   * Provides a summary of the formatter settings.
   *
   * This function corresponds to the hook_field_formatter_settings_summary()
   * function of the Drupal Field API.
   *
   * On the 'Manage Display' page of the content type administration page,
   * fields are allowed to provide a settings form.  This settings form can
   * be used to allow the site admin to define how the field should be
   * formatted.  The settings are then available for the formatter()
   * function of this class.  This function provides a text-based description
   * of the settings for the site developer to see.  It appears on the manage
   * display page inline with the field.  A field must always return a
   * value in this function if the settings form gear button is to appear.
   *
   * See the hook_field_formatter_settings_summary() function for more
   * information.
   *
   * @param $field
   * @param $instance
   * @param $view_mode
   *
   * @return string
   *   A string that provides a very brief summary of the field settings
   *   to the user.
   *
   */
  public function settingsSummary($view_mode) {
    return '';
  }
}
