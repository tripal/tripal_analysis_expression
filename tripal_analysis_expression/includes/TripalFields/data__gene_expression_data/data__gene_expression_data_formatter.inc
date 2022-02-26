<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class data__gene_expression_data_formatter extends ChadoFieldFormatter{

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
   *
   */
  protected function buildAnalysisViewer($entity) {
    $field = 'data__gene_expression_data';
    $variables = $entity->{$field}['und'][0];
    
    if (!isset($variables["features"])) {
      return NULL;
    }
    
    $analysis_id = $entity->chado_record_id;
    $feature_ids = $variables["features"]["features"];
    $biomaterials = $variables["features"]["biomaterials"];
    $file = $variables['file'];
    
    if (empty($feature_ids)) {
      return NULL;
    }
    
    return [
      '#type' => 'markup',
      '#markup' => theme('analysis_expression_plot', [
        'analysis_id' => $analysis_id,
        'biomaterials_count' => count($biomaterials),
        'features_count' => count($feature_ids),
        'file' => $file,
      ]),
    ];
  }
  
  /**
   * 
   */
  protected function buildFeatureViewer($entity) {
    $record = $entity->chado_record;
    $feature_id = $record->feature_id;
    
    // Load the necessary JS and CSS files.
    $js_path = url(drupal_get_path('module', 'tripal_analysis_expression') . '/theme/js', array('absolute' => TRUE));
    $css_path = url(drupal_get_path('module', 'tripal_analysis_expression') . '/theme/css', array('absolute' => TRUE));
    drupal_add_js('https://cdnjs.cloudflare.com/ajax/libs/d3/3.5.17/d3.min.js');
    drupal_add_js($js_path . '/expression.js');
    drupal_add_css($css_path . '/expression.css');
    
    // Get the form for specifying the network to view.
    $viewer_form = drupal_get_form('tripal_analysis_expression_feature_viewer_form', 
        $feature_id);
    $content = drupal_render($viewer_form);
    $content .= '<figure id="analysis-expression-figure"></figure>';
        
    // Return the page.
    return [
      '#type' => 'markup',
      '#markup' => $content,
    ];

  }

  /**
   *  Provides the display for a field 
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    if (count($items) == 0) {
      return;
    }
    
    $field_table = $this->instance['settings']['chado_table'];    
    if ($field_table === "analysis") {
      $element[0] = $this->buildAnalysisViewer($entity);
    }

    if ($entity->chado_table === "feature") {
      $element[0] = $this->buildFeatureViewer($entity);
    }
  }

  /**
   * Provides a summary of the formatter settings.
   */
  public function settingsSummary($view_mode) {
    return '';
  }
}
