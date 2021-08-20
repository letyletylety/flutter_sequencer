import 'package:flutter_sequencer/models/sample_descriptor.dart';

class Sfz {
  static String _buildGroup() {
    return '''
      <group>
      loop_mode=loop_continuous
    ''';
  }

  static String _buildRegion(SampleDescriptor sd) {
    return '<region>\n'
      'sample=${sd.filename}\n'
      'key=${sd.noteNumber}\n' +
      (sd.minimumNoteNumber != null ? 'lokey=${sd.minimumNoteNumber}\n' : '') +
      (sd.maximumNoteNumber != null ? 'hikey=${sd.maximumNoteNumber}\n' : '') +
      (sd.minimumVelocity != null ? 'lovel=${sd.minimumVelocity}\n' : '') +
      (sd.maximumVelocity != null ? 'hivel=${sd.maximumVelocity}\n' : '') +
      (sd.loopStartPoint != null ? 'loop_start=${sd.loopStartPoint}\n' : '') +
      (sd.loopEndPoint != null ? 'loop_end=${sd.loopEndPoint}\n' : '');
  }

  static void _setNoteRanges(List<SampleDescriptor> sampleDescriptors) {
    sampleDescriptors.sort((a, b) => a.noteNumber - b.noteNumber);
    sampleDescriptors.asMap().forEach((index, sd) {
      final prevSd = index > 0 ? sampleDescriptors[index - 1] : null;
      final nextSd =
        index < sampleDescriptors.length - 1
          ? sampleDescriptors[index + 1]
          : null;

      if (prevSd == null) {
        sd.minimumNoteNumber = 0;
      } else {
        sd.minimumNoteNumber =
          ((sd.noteNumber + prevSd.noteNumber) / 2).floor() + 1;
      }

      if (nextSd == null) {
        sd.maximumNoteNumber = 127;
      } else {
        sd.maximumNoteNumber =
          ((nextSd.noteNumber + sd.noteNumber) / 2).floor();
      }
    });
  }

  static String buildSfz(List<SampleDescriptor> sampleDescriptors) {
    _setNoteRanges(sampleDescriptors);

    final group = _buildGroup();
    final regions = sampleDescriptors.map(_buildRegion);
    final sfzString = group + regions.join();

    return sfzString;
  }
}
