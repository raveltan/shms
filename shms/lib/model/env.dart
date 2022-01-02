class EnvModel {
  late int id;
  late String date;
  late int t;
  late int h;

  EnvModel.fromJson(Map<String, dynamic> json) {
    id = json['id'];
    date = json['date'];
    t = json['t'];
    h = json['h'];
  }
}
