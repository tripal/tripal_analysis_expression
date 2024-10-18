<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__expression_features_formatter extends ChadoFieldFormatter
{

  // The default label for this field.
  public static $default_label = 'Expression Features';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__expression_features');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

  /**
   * @see ChadoFieldFormatter::settingsForm()
   *
   **/

  public function settingsForm($view_mode, $form, &$form_state)
  {

  }

  /**
   * @see ChadoFieldFormatter::View()
   *
   **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display)
  {
    $pvalue_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'obi'],
      'name' => 'p-value',
    ])->cvterm_id;
    $evidence_code_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'local'],
      'name' => 'evidence code',
    ])->cvterm_id;
    $analysis_id = $entity->chado_record->analysis_id;

    $query = chado_db_select('analysis', 'a');
    $query->fields('f', array('name', 'feature_id', 'organism_id')); //feature name and id for link
    $query->fields('fp', array('value')); // p-values (shares 'value' column in the table feature_cvtermprop)
    $query->fields('fc', array('cvterm_id'));
    $query->fields('fp2', array('value')); // expression relationship (shares 'value' column in the table feature_cvtermprop)
    $query->fields('cv', array('name')); // Keyword
    $query->join('analysisfeature', 'af', 'a.analysis_id = af.analysis_id');
    $query->join('feature', 'f', 'f.feature_id = af.feature_id');
    $query->join('feature_cvterm', 'fc', 'f.feature_id = fc.feature_id');
    $query->join('feature_cvtermprop', 'fp', 'fc.feature_cvterm_id = fp.feature_cvterm_id AND fp.type_id = :typeid', [':typeid' => $pvalue_cvterm_id]);
    $query->join('feature_cvtermprop', 'fp2', 'fp.feature_cvterm_id = fp2.feature_cvterm_id AND fp2.type_id = :typeid2', [':typeid2' => $evidence_code_cvterm_id]);
    $query->join('cvterm', 'cv', 'fc.cvterm_id = cv.cvterm_id');
    $query->condition('a.analysis_id', $analysis_id, '=');
    $total = $query->countQuery()->execute()->fetchField();
    $page = pager_default_initialize($total, 10);
    $query->range($page * 10, 10);
    $result = $query->execute();

    $header = ['Feature Name', 'Keyword', 'Relationship', 'P-value'];
    $rows = [];
    while ($record = $result->fetchAssoc()) {
      $rows[] = [
        l($record['name'], 'bio_data/' . chado_get_record_entity_by_table('feature', $record['feature_id'])),
        l($record['cv_name'], 'tripal_analysis_expression/pvalue/cvterm/' . $record['cvterm_id'] . '/' . $record['organism_id']),
        $record['fp2_value'],
        $record['value']
      ];
    }

    $pager = theme('pager', ['quantity', $total]);
    $pager = $this->ajaxifyPager($pager, $entity);

    $table = theme('table',
      [
        "header" => $header,
        "rows" => $rows,
        'pager' => $pager,
      ]);

    $element[0] =
      [
        "#type" => "markup",
        "#markup" => $table . $pager,
      ];
  }

  protected function ajaxifyPager($pager, $entity)
  {
    $field_id = 'tripal-entity-' . $entity->id . '--' . $this->field['field_name'];
    $pager = preg_replace('/href="\/.+\?page=(.+?)"/', 'href="javascript:void(0)" onclick="tripal_navigate_field_pager(\'' . $field_id . '\', \'$1\')"', $pager);
    $pager = preg_replace('/href="\/.+"/', 'href="javascript:void(0)" onclick="tripal_navigate_field_pager(\'' . $field_id . '\', 0)"', $pager);
    $pager = '<img src="/' . drupal_get_path('module', 'tripal') . '/theme/images/ajax-loader.gif" id="' . $field_id . '-spinner" class="tripal-field-ajax-spinner">' . $pager;
    return $pager;
  }

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode)
  {
    return '';
  }

}
