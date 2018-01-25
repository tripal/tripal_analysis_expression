<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class data__gene_expression_data_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Expression';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['data__gene_expression_data'];

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
   * @param $items
   * @param $display
   *
   * @throws \Exception
   * @return void
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if ($entity->chado_table === "analysis") {
      $field = 'data__gene_expression_data';
      $variables = $entity->{$field}['und'][0];

      if (!isset($variables["features"])) {
        return;
      }

      $analysis_id = $entity->chado_record_id;
      $feature_ids = $variables["features"]["features"];
      $biomaterials = $variables["features"]["biomaterials"];

      if (empty($feature_ids)) {
        return;
      }

      $element[] = [
        '#type' => 'markup',
        '#markup' => theme('analysis_expression_plot', [
          'analysis_id' => $analysis_id,
          'biomaterials_count' => count($biomaterials),
          'features_count' => count($feature_ids),
        ]),
      ];
    }


    if ($entity->chado_table === "feature") {
      $field = 'data__gene_expression_data';
      $variables = $entity->{$field}['und'][0];
      tripal_add_d3js(); // tripal uses d3 v3 not v4
      $feature_id = $entity->chado_record_id;

      if (!$variables['has_exp'] && $variables['json_exp']) {
        $content = 'No biosample libraries express this feature.';
      }
      elseif ($variables['json_exp']) {
        $default_analysis = $variables['analyses'][0]->analysis_id; // starting analysis for download link
        $hide_biomaterial_labels = $variables['hide_biomaterial_labels'];
        $json_exp = $variables['json_exp'];
        $limit_label_length = $variables['limit_label_length'];
        $expression_display = $variables['expression_display'];
        $biomaterial_display_width = $variables['biomaterial_display_width'];

        drupal_add_css(drupal_get_path('module', 'tripal_analysis_expression') . '/theme/css/expression.css');
        drupal_add_js([
          'heatMapRaw' => $json_exp,
          'maxLength' => $limit_label_length,
          'showLabels' => $hide_biomaterial_labels,
          'col' => $expression_display,
          'colWidth' => $biomaterial_display_width,
          'selectedAnalysis' => $default_analysis,
          'feature_id' => $feature_id,
        ], 'setting');
        drupal_add_js(drupal_get_path('module', 'tripal_analysis_expression') . '/theme/js/expression.js');

        $content = theme('feature_expression_plot', [
          'analyses' => $variables['analyses'],
          'default_analysis' => $default_analysis,
        ]);
      }
      else {
        $content = 'No biomaterial libraries express this feature.';
      }

      $element[] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
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
   */
  public function settingsSummary($view_mode) {
    return '';
  }
}