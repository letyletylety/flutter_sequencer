/// Contains information about a sample file. Used to build an SFZ <region>.
class SampleDescriptor {
  SampleDescriptor({
    required this.filename,
    required this.noteNumber,
    this.minimumNoteNumber,
    this.maximumNoteNumber,
    this.minimumVelocity,
    this.maximumVelocity,
    this.loopStartPoint,
    this.loopEndPoint,
  });

  String filename;
  int noteNumber;
  int? minimumNoteNumber, maximumNoteNumber;
  int? minimumVelocity, maximumVelocity;
  double? loopStartPoint, loopEndPoint;
}
