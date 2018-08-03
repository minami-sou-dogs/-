package com.example.minami.todo;


import android.content.Context;
import android.icu.text.SimpleDateFormat;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import io.realm.OrderedRealmCollection;
import io.realm.RealmBaseAdapter;

public class TaskAdapter extends RealmBaseAdapter<Task> {
    private static class ViewHolder{
        TextView deadline;
        TextView title;
    }
    public TaskAdapter(Context context, OrderedRealmCollection<Task> data) {
        super(context, data);
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder viewHolder;
        if(convertView == null){
            convertView = inflater.inflate(android.R.layout.simple_list_item_2,parent,false);
            viewHolder = new ViewHolder();
            viewHolder.deadline = (TextView)convertView.findViewById(android.R.id.text1);
            viewHolder.title =(TextView)convertView.findViewById(android.R.id.text2);
            convertView.setTag(viewHolder);
        }else{
            viewHolder = (ViewHolder)convertView.getTag();
        }
        Task task = adapterData.get(position);
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy/MM/dd");
        String formatDate = sdf.format(task.getDeadline());
        viewHolder.deadline.setText(formatDate);
        viewHolder.title.setText(task.getTitle());
        return convertView;
    }
}
