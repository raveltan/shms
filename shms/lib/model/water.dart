class WaterModel {
  late int id;
  late String date;
  late int a;

  WaterModel.fromJson(Map<String, dynamic> json) {
    id = json['id'];
    date = json['date'];
    a = json['a'];
  }
}
