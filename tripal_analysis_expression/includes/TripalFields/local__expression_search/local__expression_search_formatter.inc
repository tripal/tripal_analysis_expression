<?php

/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__expression_search_formatter extends ChadoFieldFormatter
{

  // The default label for this field.
  public static $default_label = 'Analysis Keywords';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__expression_search');

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
    $settings = $display['settings'];

    $pvalue_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'OBI'],
      'name' => 'p-value',
    ])->cvterm_id;
    $evidence_code_cvterm_id = chado_get_cvterm([
      'cv_id' => ['name' => 'local'],
      'name' => 'evidence code',
    ])->cvterm_id;
    $analysis_id = $entity->chado_record->analysis_id;

    $query = chado_db_select('analysis', 'a');
    $query->distinct();
    $query->fields('f', array('organism_id')); //feature name and id for
    $query->fields('fc', array('cvterm_id'));
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

    $header = ['Keyword'];
    $rows = [];
    while ($record = $result->fetchAssoc()) {
      $rows[] = [
        l($record['name'], 'tripal_analysis_expression/pvalue/cvterm/' . $record['cvterm_id'] . '/' . $record['organism_id']),
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

  /**
   * @see ChadoFieldFormatter::settingsSummary()
   *
   **/

  public function settingsSummary($view_mode)
  {
    return '';
  }

}
